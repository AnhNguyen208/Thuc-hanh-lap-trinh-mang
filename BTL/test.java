
import java.io.*;
import java.net.*;

public class test {
    public static void main(String[] args) throws IOException {

        if (args.length != 2) {
            System.err.println(
                "Usage: java EchoClient <host name> <port number>");
            System.exit(1);
        }

        InetAddress serverAddr = InetAddress.getByName("");
    }
}