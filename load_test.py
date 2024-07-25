import socket
import time

def send_command(sock, command):
    try:
        sock.sendall(command.encode())
        response = sock.recv(1024)
        print(f"Sent: {command.strip()}")
        print(f"Received: {response.decode()}")
    except socket.error as e:
        print(f"Connection failed: {e}")

def connect_to_server(host, port, commands):
    try:
        with socket.create_connection((host, port)) as sock:
            for command in commands:
                send_command(sock, command)
                time.sleep(0.1)  # Small delay to simulate real usage
    except socket.error as e:
        print(f"Connection failed: {e}")

def load_test(host, port, num_clients, commands):
    for i in range(num_clients):
        connect_to_server(host, port, commands)
        time.sleep(0.1)  # Small delay to avoid overwhelming the server

if __name__ == "__main__":
    commands = [
        "PASS 100\n",
        "NICK testuser\n",
        "USER testuser 0 * :Test User\n",
        "JOIN #testchannel\n",
        "PRIVMSG #testchannel :Hello everyone!\n",
        "PART #testchannel\n",
        "QUIT\n"
    ]
    load_test('127.0.0.1', 1, 10, commands)
    print("All clients have disconnected.")
