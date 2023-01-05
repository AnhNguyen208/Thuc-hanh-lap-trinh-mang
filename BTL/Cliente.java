
import java.io.*;
import java.net.*;

public class Cliente {
    private String hostName = "127.0.0.1";
    private int portNumber = 5500;
    private Socket socket;

    public void connect() {
        try {
            this.socket = new Socket(this.hostName, this.portNumber);
        } catch (Exception e) {
            System.out.println("Error connecting");
            System.out.println(e.toString() + " " + e.getLocalizedMessage());
        }
    }

    public void sendMessage(String message) throws Exception {
        PrintWriter out = new PrintWriter(this.socket.getOutputStream(), true);
        BufferedReader in = new BufferedReader(new InputStreamReader(this.socket.getInputStream()));

        Integer i = 0;
        while ((message != null) && (i < 1)) {
            out.println(message);
            System.out.println("echo: " + in.readLine());
            i++;
        }
    }

    public void disconnect() {
        try {
            this.socket.close();
        } catch (Exception e) {
            System.out.println("Error disconnect");
            System.out.println(e.toString() + " " + e.getCause());
        }
    }

    public static void main(String[] args) {
        Cliente cliente = new Cliente();
        cliente.connect();
        System.out.println("Socket " + cliente);
        try {
            cliente.sendMessage("hello");
        } catch (Exception e) {
            System.out.println("main exception");
            System.out.println(e.toString() + " " + e.getCause());
        }
        
        // try {
        //     Socket socket = new Socket("127.0.0.1", 5500);
        //     PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
        //     BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

        //     Integer i = 0;
        //     while (("message" != null) && (i < 1)) {
        //         out.println("message");
        //         System.out.println("echo: " + in.readLine());
        //         i++;
        //     }
        //     socket.close();
        // } catch (Exception e) {
        //     System.out.println(e.toString());
        // }
        cliente.disconnect();
    }
}