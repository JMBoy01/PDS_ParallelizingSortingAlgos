import pandas as pd
import matplotlib.pyplot as plt

# Laad Excel-bestand in als DataFrame
file_path = "analyse_omp.xlsx"  # Pas dit aan naar de locatie van je Excel-bestand
df = pd.read_excel(file_path, header=1)
print(df)

# Maak een lijst van unieke datasizes
datasizes = df['Datasize'].unique()

# Functie om een grafiek te maken
def plot_graph(y_col, y_label, title):
    plt.figure(figsize=(10, 6))
    
    # Plot lijn voor elke datasize
    for size in datasizes:
        subset = df[df['Datasize'] == size]
        plt.plot(subset['num_threads'], subset[y_col], label=f"Datasize {size:,}")
    
    plt.xlabel("Number of Threads")
    plt.ylabel(y_label)
    plt.title(title)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(title)
    plt.show()

# Plot grafieken
plot_graph('speedup', "Speedup", "Speedup vs Number of Threads for Different Datasizes")
plot_graph('efficiency', "Efficiency", "Efficiency vs Number of Threads for Different Datasizes")
