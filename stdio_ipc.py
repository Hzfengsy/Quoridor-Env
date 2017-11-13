#!/usr/bin/env python3

from io import StringIO
from queue import Queue
from threading import Thread
from subprocess import Popen, PIPE
import os
# import resource


def setrlimit():
    """
    m = 1024 * 1024 * 1024
    resource.setrlimit(resource.RLIMIT_AS, (m, -1))
    """


class ChildProcess():
    def __init__(self, args, stdin_save_path='/dev/null', stdout_save_path='/dev/null', stderr_save_path='/dev/null'):
        self.qmain = Queue()
        self.qthread = Queue()
        self.thread = Thread(target=self._message_thread)
        self.stdin = open(stdin_save_path, 'w')
        self.stdout = open(stdout_save_path, 'w')
        self.stderr = open(stderr_save_path, 'w')
        """
        self.child = Popen(args, bufsize=1, stdin=PIPE, stdout=PIPE,
                           stderr=self.stderr, universal_newlines=True, preexec_fn=setrlimit)
        """
        self.child = Popen(args, bufsize=1, stdin=PIPE, stdout=PIPE,
                           stderr=self.stderr, universal_newlines=True)
        self.abused = 0
        self.thread.start()

    def _message_thread(self):
        try:
            while True:
                op = self.qmain.get()
                if op['command'] == 'exit':
                    self.child.kill()
                    break
                elif op['command'] == 'send':
                    content = op['content']
                    if self.abused == 0:
                        if self.child.poll() is not None:
                            self.qthread.put('finish')
                            self.abused = -1
                            raise Exception("Runtime error.")
                        else:
                            self.abused = 2
                    self.child.stdin.write(str(content) + '\n')
                    self.child.stdin.flush()
                    self.stdin.write(str(content) + '\n')
                    self.stdin.flush()
                    self.qthread.put('finish')

                elif op['command'] == 'recv':
                    content = ''
                    if self.abused == 0:
                        if self.child.poll() is not None:
                            self.qthread.put('finish')
                            self.abused = -1
                            raise Exception("Runtime error.")
                        else:
                            self.abused = 2
                    while not content.endswith('END\n'):
                        chunk = self.child.stdout.readline()
                        if not chunk:
                            break
                        content += chunk
                    content = content[:-4]
                    self.stdout.write(content)
                    self.stdout.flush()
                    self.qthread.put(content)

                else:
                    raise Exception("unsupported command")
        except Exception as e:
            self.qthread.put(e)

    def send(self, content):
        self.qmain.put({'command': 'send', 'content': content})
        res = self.qthread.get()
        if type(res) is Exception:
            raise res
        if self.child.poll() is not None:
            raise Exception("Runtime error.")

    def recv(self, timeout):
        self.qmain.put({'command': 'recv'})
        content = self.qthread.get(timeout=timeout)
        if type(content) is Exception:
            raise content
        if self.child.poll() is not None:
            raise Exception("Runtime error.")
        return content

    def exit(self):
        self.qmain.put({'command': 'exit'})
        self.child.kill()
        self.thread.join()
        self.stdin.close()
        self.stdout.close()
        self.stderr.close()
