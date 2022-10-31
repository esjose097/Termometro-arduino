'''
main.py
José Alfredo Casal Salgueiro
ID: 00000203970
Clase main para instanciar el frame principal.
'''
from mainFrame import MainFrame
from tkinter import Tk

def main():
    root = Tk()
    root.wm_title('Pantalla principal')
    app = MainFrame(root)
    app.mainloop()

if __name__ == "__main__":
    main()