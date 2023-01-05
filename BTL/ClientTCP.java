import java.io.*;
import java.net.*;

public class ClientTCP {
    public final static String SERVER_IP = "192.168.0.106";
    public final static int SERVER_PORT = 5500;

    public static void main(String[] args) throws IOException, InterruptedException {
        Socket socket = null;
        try {
            socket = new Socket(SERVER_IP, SERVER_PORT); // Connect to server
            System.out.println("Connected: " + socket);

            OutputStream os = socket.getOutputStream();
            PrintWriter out = new PrintWriter(os);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            // for (int i = '0'; i <= '9'; i++) {
            // os.write(i); // Send each number to the server
            // // int ch = is.read(); // Waiting for results from server
            // System.out.print(is.read() + "\n"); // Display the results received from the
            // server
            // Thread.sleep(200);
            // }
            out.println("HAHAHAHAH");
            // System.out.print(in.readLine() + "\n");

        } catch (IOException ie) {
            System.out.println("Can't connect to server");
        } finally {
            if (socket != null) {
                socket.close();
            }
        }
    }
}