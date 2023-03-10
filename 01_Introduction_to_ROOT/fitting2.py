import sys
import math
import numpy as np
import matplotlib.pyplot as plt
from array import array
from ROOT import *

a=1.
b=2.
nPoints = 10
#create a list of points
x = np.arange(start=1, stop=9, step=1)

#calculcate means
#clone the points - one line for each y value at hiven point.
# it is enough to clone x, the following arrays will be broadcasted nt the fly
y_means = a+b*x
y_means = y_means.reshape((-1,1))
y_means = np.broadcast_to(y_means, (len(y_means), nPoints))

#define sigmas
y_sigmas = np.array([ 1., 1., 2., 3., 1., 2.,  3., 1.]).reshape((-1,1))
#y_sigmas = np.full_like(y_means, 0.01) # test setting with very low sigma

#draw an array of random numbers with nPoints rows
values = np.random.default_rng().normal(loc=y_means, scale=y_sigmas)

y = np.mean(values, axis=1)
e = np.std(values, axis=1, ddof=1)
print('Values: ', values)
print("X values:\n",x)
print("Mean Y values:\n",y)
print("Y standard deviation:\n",e)

w = 1.0/e**2

S = np.sum(w)
Sx = np.sum(w*x)
Sy = np.sum(w*y)
Sxx = np.sum(w*x*x)
Sxy = np.sum(w*x*y)
  
D = S*Sxx-Sx*Sx
a_fit = (S*Sxy-Sx*Sy)/D
b_fit = (Sxx*Sy-Sx*Sxy)/D

d = (y-(a_fit*x+b_fit))/e
chi2 = np.sum(d)

print("Fitted values: a = {}, b = {}".format(a_fit,b_fit))
print("True values: a = {}, b = {}".format(a,b))
print ('chi2/NDF=',chi2/(len(x)-2.))

plt.errorbar(x, y, yerr=e, marker=".", linestyle="none", markerfacecolor="red", markeredgecolor="red", markersize=10)
plt.plot(x, (a_fit*x+b_fit), "b");
plt.xlabel("x")
plt.ylabel("y")

#gr = TGraph(len(x), x, y)

xx = array('d')
xx.fromlist(x.tolist())

yy = array('d')
yy.fromlist(y.tolist())

ee = array('d')
ee.fromlist(e.tolist())

gr = TGraphErrors(len(x), xx, yy, ee)
gr.SetMarkerColor(4)
gr.SetMarkerStyle(21)

c = TCanvas('cFitting','cFitting',600,600)
c.Draw()
hDummy = TH1D('hDummy','Prosta',100,0.,10.)
hDummy.SetMaximum(12.)
hDummy.SetMinimum(0.)
hDummy.GetXaxis().SetTitle("x")
hDummy.GetYaxis().SetTitle("y")
hDummy.SetStats(0)
hDummy.DrawCopy()
gr.Draw("s p")
