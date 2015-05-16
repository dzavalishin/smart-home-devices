// Simple UDP echo back application for udp port 7 (echo port)

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

public class udp_echo {
    static final int BUFF_SIZE = 1024;

    public static void main(String[] args) {
        DatagramSocket socket;
        DatagramPacket packet = new DatagramPacket(new byte[BUFF_SIZE], BUFF_SIZE);
        
        System.out.println("UDP echo server startet, listening on UDP port 7. Press <ctrl>+<c> to abort");
        
        try {
            // create a socket on udp port 7 (echo port)
           socket = new DatagramSocket(7);
        } catch (SocketException exception) {
            System.out.println(exception);
            return;
        }

        while (true) {
            // this is the echo back
            try {
                socket.receive(packet);
                socket.send(packet);
            } catch (IOException io_exception) {
                System.out.println(io_exception);
            }
        }
    }
}

