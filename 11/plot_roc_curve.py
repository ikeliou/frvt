#!/usr/bin/python
import numpy as np
from sklearn.metrics import roc_curve, auc

import matplotlib as mpl
#mpl.use('Agg')
import matplotlib.pyplot as plt
def plot_roc_curve(y, scores, save_file):
	#y = np.array([0, 0, 1, 1])
	#scores = np.array([0.1, 0.4, 0.35, 0.8])
	fpr, tpr, _ = roc_curve(y, scores)
	roc_auc = auc(fpr, tpr)

	fig = plt.figure()
	lw = 2
	plt.plot(fpr, tpr, color='darkorange', lw=lw, label='ROC curve (area = %0.2f)' % roc_auc)
	plt.plot([0, 1], [0, 1], color='navy', lw=lw, linestyle='--')
	plt.xlim([0.0, 1.0])
	plt.ylim([0.0, 1.05])
	plt.xlabel('False Positive Rate')
	plt.ylabel('True Positive Rate')
	plt.title('Receiver operating characteristic example')
	plt.legend(loc="lower right")
	fig.savefig(save_file)
	#plt.show()
