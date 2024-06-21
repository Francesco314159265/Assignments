import subprocess
import sys
import os
import numpy as np
from matplotlib import pyplot as plt



class PowerLaw():
    def __init__(self , xmin : float , alpha : float):
        self.xmin = xmin
        self.alpha = alpha

    def pdf(self , X : np.ndarray) -> np.ndarray:
        return (self.alpha - 1) / self.xmin * (X / self.xmin) ** (- self.alpha)
    
    def cdf(self , X : np.ndarray) -> np.ndarray:
        return 1 - ( X / self.xMin) ** (-self.alpha + 1)



class LogNorm():
    def __init__(self , mean : float , std : float):
        self.mean = mean
        self.std = std

    def pdf(self , X : np.ndarray) -> np.ndarray:
        normalized = (np.log(X) - self.mean) / self.std
        return 1 / ( X * np.sqrt(2 * np.pi) * self.std) * np.exp(- normalized * normalized  / 2)
    






# command = "Kersten.exe"
# process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

# while True:
#     out = process.stdout.readline()
#     if not out: 
#         print("Finished")
#         break
#     if out != '':
#         sys.stdout.write(out.decode('ascii'))
#         sys.stdout.flush()





    
filename = "files/data.txt"
directory = os.path.dirname(__file__)
abs_file_path = os.path.join(directory, filename)



file = open(abs_file_path , "r")

N = int(file.readline())
data = file.readline().split(",")
pLow = PowerLaw(float(data[0]) , float(data[1]))

data = file.readline().split(",")
logNorm = LogNorm(float(data[0]) , float(data[1]))

x = np.empty((N , ))
for i , line in enumerate(file):
    data = line.split(",")
    x[i] = float(data[0]) 







Nbins = 50
bin_edges = np.logspace(np.log10(min(x)), np.log10(max(x)), Nbins)
hist , edges = np.histogram(x , bin_edges , density = True )
bin_centers = 0.5 * (edges[:-1] + edges[1:])


plt.figure(figsize=(10, 6))
ax = plt.axes()

for count, start, end in zip(hist, edges[:-1], edges[1:]):
    plt.plot([start, end], [count, count], marker='o', color='blue')


xLog = np.linspace(min(x) , pLow.xmin , 100)
xPow = np.linspace( pLow.xmin , max(x) , 100)

yLog = logNorm.pdf(xLog )
yLog *= (x[x < pLow.xmin]).size / x.size 
yPow = pLow.pdf(xPow) 
yPow *= (x[x >= pLow.xmin]).size / x.size 

plt.plot(xLog, yLog, 'r-', lw=2, label='Lognormal PDF')
plt.plot(xPow, yPow, 'r-', lw=2, label='PowerLaw  PDF')



plt.xscale('log')
plt.yscale('log')  

plt.xlabel('Value')
plt.ylabel('Density')
plt.title('Log-Log Histogram')
plt.grid(True, which="both", ls="--", linewidth=0.5)
plt.show()