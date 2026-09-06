import tkinter as tk
import time

class KeyTestApp:
    def __init__(self, root):
        self.root = root
        self.root.title("按键测试工具")
        self.root.geometry("400x300")
        self.press_time = None
        self.press_key = None
        self.show_main()

    def clear(self):
        for w in self.root.winfo_children():
            w.destroy()

    def show_main(self):
        self.clear()
        tk.Label(self.root, text="按键测试工具", font=("Arial", 20)).pack(pady=40)
        tk.Button(self.root, text="按住时长测试", font=("Arial", 14),
                  command=self.show_test, width=15, height=2).pack()

    def show_test(self):
        self.clear()
        self.press_time = None
        self.press_key = None

        tk.Label(self.root, text="按住时长测试", font=("Arial", 16)).pack(pady=10)
        self.result_label = tk.Label(self.root, text="按下任意键...", font=("Arial", 14))
        self.result_label.pack(pady=20)
        self.detail_label = tk.Label(self.root, text="", font=("Arial", 12))
        self.detail_label.pack(pady=5)

        tk.Button(self.root, text="返回主界面", font=("Arial", 12),
                  command=self.show_main, width=12).pack(pady=30)

        self.root.bind("<KeyPress>", self.on_key_press)
        self.root.bind("<KeyRelease>", self.on_key_release)
        self.root.focus_set()

    def on_key_press(self, event):
        if self.press_time is None:
            self.press_time = time.time()
            self.press_key = event.keysym
            self.result_label.config(text=f"按住中: {self.press_key}")
            self.detail_label.config(text="")

    def on_key_release(self, event):
        if self.press_time is not None and event.keysym == self.press_key:
            duration = time.time() - self.press_time
            self.result_label.config(text=f"键名: {self.press_key}")
            self.detail_label.config(text=f"时长: {duration:.3f} 秒")
            self.press_time = None
            self.press_key = None

if __name__ == "__main__":
    root = tk.Tk()
    app = KeyTestApp(root)
    root.mainloop()
