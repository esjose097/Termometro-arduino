'''
mainFrame.py
José Alfredo Casal Salgueiro
ID: 00000203970
Fecha: 26/05/2021
'''
# Libreria que se utilizará para el serial.
import serial
from estadisticas import Estadisticas
# Libreria que se utilizará para la conexión a la base de datos.
import mysql.connector as pysql
# Libreria que se utilizará para manejar las fechas.
import datetime as pydate
# Libreria que se utilizará para manejar las interfaces gráficas.
from tkinter import *
# Libreria que se utilizará para manejar Hilos.
import threading
# Libreria que nos ayudará con las estadisticas.
import statistics as estadisticas
import time
# Libreria para estadisticas
from matplotlib import  pyplot as plt
#Libreria que nos ayudará con algunos cálculos
import numpy as np


class MainFrame(Frame):
    def __init__(self, master=None):
        super().__init__(master, width=420, height=270)
        self.master = master
        self.pack()
        #Se crea un hilo para leer los datos del arduino de manera constante
        self.hiloA = threading.Thread(target=self.leeArduino, daemon=True)
        '''
        Se declara el arduino mediante el modulo "serial" y se vincula a nuestro
        arduino en el puerto COM3
        '''
        self.arduino = serial.Serial('COM3', 9600, timeout=5)
        #Le damos un ligero descanso al sitema para establecer la conexión
        time.sleep(1)
        '''
        Se establece una conexión a la base de datos.
        '''
        self.pydb = pysql.connect(host='localhost', user='root', password='sesamo', database='termometro')
        '''
        Se establece el objeto cursor.
        '''
        self.mycursor = self.pydb.cursor()
        #Variable para la altura
        self.temperaturaActual = StringVar()
        #Variable de rango de edad
        self.rangoEdad = StringVar()
        #Variable para el rango de temperatura corporal
        self.corporalRange = StringVar()
        #Método que se encarga de cargar los widgets de la interfaz
        self.create_widgets()
        #Se inicializa el hilo
        self.hiloA.start()

    def create_widgets(self):
        '''
        Función encargada de establecer los valores de los widgets del frame principal
        :return:
        '''
        #Se crean las etiquetas para la  temperatura corporal
        Label(self, text="Temperaturas: ", font=("Arial", "11", "bold")).place(x=30, y=20)
        Label(self, textvariable=self.temperaturaActual, font=("Arial", "11", "bold")).place(x=140, y=20)
        #Se crean las etiquetas para el rango de edad
        Label(self, text="Rango edad: ", font=("Arial", "11", "bold")).place(x=30, y=50)
        Label(self, textvariable=self.rangoEdad, font=("Arial", "11")).place(x=140, y=50)
        #Se crea las etiquetas para el rango de temperatura aceptado
        Label(self, text="Rango de temperatura: ", font=("Arial", "11", "bold")).place(x=30, y=110)
        Label(self, textvariable=self.corporalRange, font=("Arial", "11")).place(x=190, y=110)
        #Botón que se encarga de desplegar la gráfica de barras mediante la función @despliegaEstadisticas()
        Button(self, text="Ir a estadisticas", command=self.despliegaEstadisticas, font=("Helvetica", "11")).place(x=30,
                                                                                                                   y=230)

    def despliegaEstadisticas(self):
        '''
        Función de encargada de desplegar la gráfica de barras.
        :return:
        '''
        #Se realiza una consulta a la bd con los valores
        self.mycursor.execute('SELECT temperatura FROM termometro')
        #Se guardan los valores obtenidos
        resultados = self.mycursor.fetchall()
        #Lista para separar los valores puesto que estos vienen en tuplas.
        datos = []
        #Se crea una instancia del módulo matplotpy
        self.plt = plt
        contador = 0
        for x in resultados:
            datos.append(x[0])
        y = np.zeros(len(datos))
        for i in range(35):
            y[contador] = i
        self.plt.ion()
        self.plt.bar(y, datos)

    def insert(self, temp):
        '''
        Esta función realiza una alta a la base de datos.
        :param temp: Temperatura recibida por el sensor.
        :return:
        '''
        fecha = self.obtenFechaActual()
        sql = "INSERT INTO termometro (fecha, temperatura) VALUES (%s, %s)"
        values = (fecha, temp)
        self.mycursor.execute(sql, values)
        self.pydb.commit()

    def obtenFechaActual(self):
        '''
        Esta función se encarga de regresar la fecha actual del sistema
        :return: fecha(Fecha actual del sistema).
        '''
        fecha = pydate.datetime.now()
        return fecha

    def leeArduino(self):
        '''
        Función encargada de leer el puerto serial del arduino
        y en caso de detectar una distancia valida realizar una
        alta a la base de datos mediante la función @insert()
        :return: Esta función no devuelve ningun valor.
        '''
        while True:
            self.dato = self.arduino.readline()[:-2]
            datosC = str(self.dato)
            datosC = datosC.replace("b'", '').replace("'", '')
            if datosC != "":
                if datosC == '0' or datosC == '1' or datosC == '2':
                    self.obtenRango(datosC)
                    self.arduino.write(b'tm')
                else:
                    datosF = float(datosC)
                    self.insert(datosF)
                    self.temperaturaActual.set(datosF)
                    self.arduino.write(b'st')
            else:
                self.arduino.write(b'st')

    def obtenRango(self, datoC):
        '''
        Función encargada de cambiar los estados de las variables rangoEdad
        y corporalRange.
        :param dato:
        :return:
        '''
        if datoC == '0':
            self.rangoEdad.set('Niños')
            self.corporalRange.set('Minimo: 36.6°C Máximo: 37.2°C')
        elif datoC == '1':
            self.rangoEdad.set('Persona adulta')
            self.corporalRange.set('Minimo: 36.1°C Máximo: 37.5°C')
        elif datoC == '2':
            self.rangoEdad.set('Adulto de tercera edad')
            self.corporalRange.set('Minimo: 35.2°C Máximo: 36.2°C')
