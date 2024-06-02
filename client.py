import socket
import time
import tkinter as tk
from tkinter import messagebox

WINDOWS_HOST = '192.168.29.99'  # Windows server IP address
LINUX_HOST = '192.168.56.101'  # Linux server IP address
PORT = 8080  # Server port

class ClientInterface:
    def __init__(self, master):
        self.master = master
        self.master.title("Client Interface")
        self.master.configure(bg="#FAF0E6")  # Set the background color to match the image

        # Create and place widgets
        self.connect_button_windows = tk.Button(self.master, text="Connect to Windows", command=self.connect_to_windows,
                                                bg="#800000", fg="white", activebackground="#A52A2A", font=('Arial', 12, 'bold'),
                                                borderwidth=0, relief="flat")
        self.connect_button_windows.pack(padx=10, pady=(50, 5))

        self.connect_button_linux = tk.Button(self.master, text="Connect to Linux", command=self.connect_to_linux,
                                              bg="#800000", fg="white", activebackground="#A52A2A", font=('Arial', 12, 'bold'),
                                              borderwidth=0, relief="flat")
        self.connect_button_linux.pack(padx=10, pady=5)

        self.disconnect_button = tk.Button(self.master, text="Disconnect", command=self.disconnect_from_server,
                                           state=tk.DISABLED, bg="#800000", fg="white", activebackground="#A52A2A",
                                           font=('Arial', 12, 'bold'), borderwidth=0, relief="flat")
        self.disconnect_button.pack(padx=10, pady=5)

        self.connection_label = tk.Label(self.master, text="Connection Status: Disconnected", font=('Arial', 14, 'bold'),
                                         fg="red", bg="#FAF0E6")
        self.connection_label.pack(padx=10, pady=5)

        self.duration_label = tk.Label(self.master, text="", font=('Arial', 14, 'italic'), bg="#FAF0E6")  # Initially empty
        self.duration_label.pack(padx=10, pady=(5, 50))

        self.connection_time = None
        self.client_socket = None

    def connect_to_windows(self):
        if self.is_server_active(WINDOWS_HOST):
            self.connect_to_server(WINDOWS_HOST)
        else:
            self.show_error("Windows Server not responding.")

    def connect_to_linux(self):
        if self.is_server_active(LINUX_HOST):
            self.connect_to_server(LINUX_HOST)
        else:
            self.show_error("Linux Server not responding.")

    def is_server_active(self, host):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(2)  # Set a timeout for the connection attempt
            s.connect((host, PORT))
            s.close()
            return True
        except socket.error:
            return False

    def connect_to_server(self, host):
        self.connection_label.config(text="Connection Status: Connecting...", fg="blue")

        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.settimeout(5)  # Set a timeout for the connection attempt
            self.client_socket.connect((host, PORT))
            self.connection_time = time.time()
            self.connection_label.config(text="Connection Status: Connected", fg="green")
            self.connect_button_windows.config(state=tk.DISABLED)
            self.connect_button_linux.config(state=tk.DISABLED)
            self.disconnect_button.config(state=tk.NORMAL)

            # Display connection time
            connection_time_str = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(self.connection_time))
            self.duration_label.config(text=f"Connected at: {connection_time_str}", font=('Arial', 14, 'italic'))

        except socket.error as e:
            self.show_error(f"Error: {e}")

    def disconnect_from_server(self):
        if self.client_socket:
            self.client_socket.close()
            self.client_socket = None
        self.connect_button_windows.config(state=tk.NORMAL)
        self.connect_button_linux.config(state=tk.NORMAL)
        self.disconnect_button.config(state=tk.DISABLED)
        self.connection_label.config(text="Connection Status: Disconnected", fg="red")

        if self.connection_time:
            # Display disconnection time and duration
            disconnection_time = time.time()
            disconnection_time_str = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(disconnection_time))
            duration = disconnection_time - self.connection_time
            self.duration_label.config(text=f"Connected at: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(self.connection_time))}\n\nDisconnected at: {disconnection_time_str}\n\nConnection Duration: {duration:.2f} seconds", font=('Arial', 14, 'italic'))
        else:
            self.duration_label.config(text="")

    def show_error(self, message):
        self.connection_label.config(text="Connection Status: Error - Server not responding", fg="red")
        self.duration_label.config(text="")  # Clear the duration label if there's an error
        messagebox.showerror("Connection Error", message)

if __name__ == "__main__":
    root = tk.Tk()
    app = ClientInterface(root)
    root.mainloop()