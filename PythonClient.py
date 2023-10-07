
import socket
import pygame

pygame.init()
display = pygame.display.set_mode((300,300))


UDP_IP = "192.168.4.1"
UDP_PORT = 9999
MESSAGE = b"n"

sock = socket.socket(socket.AF_INET, # Internet 
                     socket.SOCK_DGRAM) # UDP
while True:
    for event in pygame.event.get():
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_w:
                MESSAGE = b"w"
            elif event.key == pygame.K_s:
                MESSAGE = b"s"
            elif event.key == pygame.K_a:
                MESSAGE = b"a"
            elif event.key == pygame.K_d:
                MESSAGE = b"d"
            elif event.key == pygame.K_q:
                MESSAGE = b"q"
            elif event.key == pygame.K_e:
                MESSAGE = b"e"
            sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))

        elif event.type == pygame.KEYUP:
            MESSAGE = b"n"
            sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))