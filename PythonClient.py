import socket
import pygame
import array

pygame.init()
display = pygame.display.set_mode((300,300))


UDP_IP = "192.168.4.1"
UDP_PORT = 9999
class DATA:
    x = 128
    y = 128
    a = 128
    b = 128
    ch1 = 128
    ch2 = 128
    ch3 = 128
    ch4 = 128


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP

while True:
    for event in pygame.event.get():
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_w:
                DATA.x = 255
            elif event.key == pygame.K_s:
                DATA.x = 0
            elif event.key == pygame.K_a:
                DATA.y = 0
            elif event.key == pygame.K_d:
                DATA.y = 255
            ArrDATA = array.array('i',[DATA.x, DATA.y, DATA.a, DATA.b, DATA.ch1, DATA.ch2, DATA.ch3, DATA.ch4])
            print(ArrDATA)
            sock.sendto(ArrDATA, (UDP_IP, UDP_PORT))

        elif event.type == pygame.KEYUP:
            if event.key == pygame.K_w:
                DATA.x = 128
            elif event.key == pygame.K_s:
                DATA.x = 128
            elif event.key == pygame.K_a:
                DATA.y = 128
            elif event.key == pygame.K_d:
                DATA.y = 128
            ArrDATA = array.array('i',[DATA.x, DATA.y, DATA.a, DATA.b, DATA.ch1, DATA.ch2, DATA.ch3, DATA.ch4])
            print(ArrDATA)
            sock.sendto(ArrDATA, (UDP_IP, UDP_PORT))