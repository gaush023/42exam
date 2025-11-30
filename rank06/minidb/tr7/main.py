#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import socket
import threading
import time
import random
import string
import sys

HOST = '127.0.0.1'
PORT = 1111  # mini_db サーバを起動したポートに合わせて
NUM_CLIENTS = 100   # 同時クライアント数
OPS_PER_CLIENT = 10  # 各クライアントのリクエスト回数
MAX_KEY_LEN = 16
MAX_VAL_LEN = 128

def rand_str(n):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=n))

def client_thread(id):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(5)
        s.connect((HOST, PORT))
    except Exception as e:
        print(f"[{id}] connect failed: {e}")
        return

    for i in range(OPS_PER_CLIENT):
        kind = random.choice(['POST', 'GET', 'DELETE'])
        key = rand_str(random.randint(1, MAX_KEY_LEN))
        val = rand_str(random.randint(1, MAX_VAL_LEN)) if kind == 'POST' else ''
        if kind == 'POST':
            msg = f"POST {key} {val}\n"
        elif kind == 'GET':
            msg = f"GET {key}\n"
        else:  # DELETE
            msg = f"DELETE {key}\n"

        try:
            s.sendall(msg.encode())
        except Exception as e:
            print(f"[{id}] send error: {e}")
            break

        try:
            # 応答を受け取る (適当に 1024byte まで)
            resp = s.recv(1024)
            if not resp:
                print(f"[{id}] closed by server")
                break
            print(f"[{id}] {msg.strip()} -> {resp.decode().strip()}")
        except Exception as e:
            print(f"[{id}] recv error: {e}")
            break

        # ランダムに待ち時間を入れて遅延クライアントをシミュレート
        time.sleep(random.uniform(0, 0.1))

    try:
        s.close()
    except:
        pass

def main():
    threads = []
    for i in range(NUM_CLIENTS):
        t = threading.Thread(target=client_thread, args=(i,))
        t.start()
        threads.append(t)
        time.sleep(random.uniform(0, 0.01))  # 接続のタイミングをずらす

    for t in threads:
        t.join()

    print("done")

if __name__ == '__main__':
    if len(sys.argv) >= 2:
        NUM_CLIENTS = int(sys.argv[1])
    if len(sys.argv) >= 3:
        OPS_PER_CLIENT = int(sys.argv[2])
    print(f"running: clients={NUM_CLIENTS}, ops/client={OPS_PER_CLIENT}")
    main()
