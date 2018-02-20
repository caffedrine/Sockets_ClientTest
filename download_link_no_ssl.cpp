    std::string downloadLink(std::string url)
    {
        socklen_t sockfd;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        
        uint16_t portno;
        ssize_t n;
        
        char buffer[512];
        
        portno = 80;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            setLastError("Can't open linux socket");
            return "";
        }
        
        std::string hstnm = getHostFromUrl(url);
        const char *hstname = hstnm.c_str();
        
        server = gethostbyname( hstname );
        if (server == nullptr)
        {
            setLastError("Can't resolve hostname to ip");
            return "";
        }
        
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        
        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, sizeof(server->h_addr));
        serv_addr.sin_port = htons(portno);
        
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        {
            std::string str = std::string(hstname, strnlen(hstname, 32));
            setLastError("Can't connect to host " + str );
            return "";
        }
        
        // Build get request
        std::string path = removeSubstrs(url, "http://");
        path = removeSubstrs(path, "https://");
        path = removeSubstrs(path, hstname);
        
        if(path.find('/') == std::string::npos)
            path = "/";
        
        std::string reqHeader = "GET " + path  + " HTTP/1.0\r\n";
        reqHeader += "Host: " + getHostFromUrl( url ) + "\r\n";
        reqHeader += "User-Agent: Safari/Chrome MacBook PRO A1337\r\n";
        //reqHeader += "Content-Length: "; reqHeader += "\r\n";
        reqHeader += "\r\n";
        
        cout << reqHeader << endl;
        
        n = write(sockfd,reqHeader.c_str(),strlen(reqHeader.c_str()));
        if (n < 0)
        {
            setLastError("Can't write data to socket");
            close(sockfd);
            return "";
        }
        
        memset(buffer, 0, sizeof(buffer));
        n = read(sockfd, buffer, 500);
        if (n < 0)
        {
            setLastError("Can't read from socket");
            close(sockfd);
            return "";
        }
        
        return std::string(buffer);
    }
