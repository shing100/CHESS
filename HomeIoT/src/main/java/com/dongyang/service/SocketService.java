package com.dongyang.service;

import com.google.gson.JsonObject;
import org.springframework.stereotype.Service;
import java.io.*;
import java.net.*;

/**
 * Created by HeemangHan on 2016. 8. 5..
 */
@Service
public class SocketService {
    public String sendRequest(String ipAddress, String message) {
        Socket socket = new Socket();
        int port = 9999;
        int timeout = 2000;

        SocketAddress socketAddress = new InetSocketAddress(ipAddress, port);
        InputStream inputStream = null;
        BufferedReader bufferedReader = null;
        OutputStream outputStream = null;
        StringBuffer buffer = new StringBuffer();
        String inputLine = null;
        String rcvMessage = null;

        try {
            socket.setSoTimeout(timeout);
            socket.connect(socketAddress, timeout);

            outputStream = socket.getOutputStream();
            outputStream.write(message.getBytes());

            inputStream = socket.getInputStream();
            bufferedReader = new BufferedReader(new InputStreamReader(inputStream));

            while((inputLine = bufferedReader.readLine()) != null) {
                buffer.append(inputLine);
                buffer.append("\r\n");
            }

            rcvMessage = buffer.toString();
        } catch (SocketTimeoutException e) {
        } catch (ConnectException e) {
        } catch (IOException e) {
        } finally {
            try {
                if(socket != null)
                    socket.close();
                if(outputStream != null)
                    outputStream.close();
                if(inputStream != null)
                    inputStream.close();
                if(bufferedReader != null)
                    bufferedReader.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return rcvMessage;
    }
}
