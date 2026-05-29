import matplotlib
matplotlib.use('TkAgg')

import pandas as pd
import matplotlib.pyplot as plt

# ---------------- READ CSV ----------------
df = pd.read_csv("report.csv")

# ---------------- TOTALS ----------------
income = df[df['Type'] == 'income']['Amount'].sum()
expense = df[df['Type'] == 'expense']['Amount'].sum()
savings = income - expense

# ---------------- CATEGORY DATA ----------------
expense_df = df[df['Type'] == 'expense']

category_expense = (
    expense_df.groupby('Category')['Amount']
    .sum()
)

# ---------------- FIGURE ----------------
fig, ax = plt.subplots(
    1,
    2,
    figsize=(14,6)
)

# ==================================================
# BAR CHART
# ==================================================

labels = ['Income', 'Expense', 'Savings']
values = [income, expense, savings]

colors = ['green', 'red', 'blue']

bars = ax[0].bar(
    labels,
    values,
    color=colors,
    width=0.6
)

# Value labels
for bar in bars:

    height = bar.get_height()

    ax[0].text(
        bar.get_x() + bar.get_width()/2,
        height + 1000,
        f"{height:.0f}",
        ha='center',
        fontsize=11,
        fontweight='bold'
    )

ax[0].set_title(
    "Finance Overview",
    fontsize=18,
    fontweight='bold'
)

ax[0].set_ylabel("Amount")
ax[0].grid(
    axis='y',
    linestyle='--',
    alpha=0.4
)

# ==================================================
# PIE CHART
# ==================================================

pie_colors = [
    '#ff9999',
    '#66b3ff',
    '#99ff99',
    '#ffcc99',
    '#c2c2f0',
    '#ffb3e6'
]

ax[1].pie(
    category_expense,
    labels=category_expense.index,
    autopct='%1.1f%%',
    startangle=140,
    colors=pie_colors
)

ax[1].set_title(
    "Expense Categories",
    fontsize=18,
    fontweight='bold'
)

# ---------------- FINAL ----------------
plt.tight_layout()

plt.show()