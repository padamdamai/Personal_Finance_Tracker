import matplotlib
matplotlib.use('TkAgg')

import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("report.csv")

income = df[df['Type'] == 'income']['Amount'].sum()
expense = df[df['Type'] == 'expense']['Amount'].sum()

labels = ['Income', 'Expense']
values = [income, expense]

plt.bar(labels, values)
plt.title("Finance Overview")
plt.xlabel("Type")
plt.ylabel("Amount")

plt.show()   # This will now open window 🎉