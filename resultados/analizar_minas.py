import glob
import os
import sys
import matplotlib.pyplot as plt
import pandas as pd

# 1. Validar que existan los 6 archivos requeridos antes de procesar
archivos_esperados = {
    "resultados_Terran_Izquierda.csv",
    "resultados_Terran_Derecha.csv",
    "resultados_Protoss_Izquierda.csv",
    "resultados_Protoss_Derecha.csv",
    "resultados_Zerg_Izquierda.csv",
    "resultados_Zerg_Derecha.csv",
}

archivos_encontrados = {os.path.basename(f) for f in glob.glob("resultados_*.csv")}
faltantes = sorted(archivos_esperados - archivos_encontrados)

if faltantes:
  print(f"Faltan {len(faltantes)} de los 6 archivos requeridos. Aún no se generarán gráficas:")
  for f in faltantes:
    print(f"  - {f}")
  sys.exit(0)

archivos_csv = sorted(list(archivos_esperados))

# 2. Cargar y consolidar los 6 archivos CSV
lista_df = []
for ruta in archivos_csv:
  df_temp = pd.read_csv(ruta)
  nombre_base = os.path.basename(ruta).replace(".csv", "")
  partes = nombre_base.split("_")
  raza = partes[1] if len(partes) > 1 else "Desconocida"
  df_temp["Raza"] = raza
  lista_df.append(df_temp)

df = pd.concat(lista_df, ignore_index=True)

# 3. Limpiar duplicados por (Raza, Lado, ID_Mina) quedándose con el menor promedio
df = df.sort_values("Promedio_Frames", ascending=True).drop_duplicates(
    subset=["Raza", "Lado", "ID_Mina"], keep="first"
)

# 4. Obtener los 4 minerales más eficientes (menor Promedio_Frames) por Raza y Lado
top4 = (
    df.sort_values(["Raza", "Lado", "Promedio_Frames"], ascending=[True, True, True])
    .groupby(["Raza", "Lado"], as_index=False)
    .head(4)
)

# Guardar tabla resumen completa en CSV y mostrar en consola
top4.to_csv("tabla_top4_eficiencia.csv", index=False)
print("=== TOP 4 MINERALES MÁS EFICIENTES POR RAZA Y LADO ===")
print(
    top4[["Raza", "Lado", "ID_Mina", "Pos_X", "Pos_Y", "Promedio_Frames"]].to_string(
        index=False
    )
)

# 5. Resumen estadístico comparativo (Promedio del Top 4 por Raza y Lado)
resumen_razas = (
    top4.groupby(["Raza", "Lado"])["Promedio_Frames"]
    .agg(Promedio_Top4="mean", Min_Frames="min", Max_Frames="max", Desv_Std="std")
    .reset_index()
)
resumen_razas.to_csv("resumen_comparativo_razas.csv", index=False)
print("\n=== RESUMEN COMPARATIVO POR RAZA Y LADO (TOP 4) ===")
print(resumen_razas.round(2).to_string(index=False))

# ==========================================
# GRÁFICA 1: Comparación del Top 4 por Raza y Lado
# ==========================================
plt.style.use("seaborn-v0_8-whitegrid")
razas = sorted(top4["Raza"].unique())

fig, axes = plt.subplots(1, len(razas), figsize=(5 * len(razas), 5), sharey=True)
if len(razas) == 1:
  axes = [axes]

colores = {"Izquierda": "#2b5c8f", "Derecha": "#d95f02"}

for ax, raza in zip(axes, razas):
  sub = top4[top4["Raza"] == raza].copy()
  sub["Etiqueta"] = (
      sub["Lado"].str[:3] + " - ID " + sub["ID_Mina"].astype(str)
  )
  bars = ax.bar(
      sub["Etiqueta"],
      sub["Promedio_Frames"],
      color=[colores.get(l, "#555555") for l in sub["Lado"]],
      edgecolor="black",
      linewidth=0.7,
  )
  ax.set_title(f"Raza: {raza}", fontsize=12, fontweight="bold")
  ax.set_xlabel("Formación y ID de Mineral", fontsize=10)
  ax.tick_params(axis="x", rotation=45)

  for bar in bars:
    h = bar.get_height()
    ax.annotate(
        f"{h:.1f}",
        xy=(bar.get_x() + bar.get_width() / 2, h),
        xytext=(0, 3),
        textcoords="offset points",
        ha="center",
        va="bottom",
        fontsize=8,
    )

axes[0].set_ylabel("Promedio de Frames por Extracción (Menor = Mejor)", fontsize=10)
fig.suptitle(
    "Top 4 Minerales Más Eficientes por Raza y Lado del Mapa",
    fontsize=14,
    fontweight="bold",
)
plt.tight_layout()
plt.savefig("grafica_top4_por_raza.png", dpi=300)
plt.close()

# ==========================================
# GRÁFICA 2: Eficiencia Global (Izquierda vs Derecha por Raza)
# ==========================================
pivot_prom = resumen_razas.pivot(
    index="Raza", columns="Lado", values="Promedio_Top4"
)
ax = pivot_prom.plot(
    kind="bar",
    figsize=(8, 5),
    color=["#2b5c8f", "#d95f02"],
    edgecolor="black",
    width=0.65,
)
plt.title(
    "Comparación de Eficiencia Media (Top 4): Izquierda vs. Derecha",
    fontsize=13,
    fontweight="bold",
)
plt.ylabel("Promedio de Frames (Menor = Más Eficiente)", fontsize=11)
plt.xlabel("Raza", fontsize=11)
plt.xticks(rotation=0)
plt.legend(title="Formación del Mapa")

for container in ax.containers:
  ax.bar_label(container, fmt="%.2f", padding=3, fontsize=9)

plt.tight_layout()
plt.savefig("grafica_comparacion_lados.png", dpi=300)
plt.close()

# ==========================================
# GRÁFICA 3: Mapa Espacial de Eficiencia (Coordenadas X, Y)
# ==========================================
plt.figure(figsize=(9, 6))
scatter = plt.scatter(
    df["Pos_X"],
    df["Pos_Y"],
    c=df["Promedio_Frames"],
    cmap="RdYlGn_r",
    s=140,
    edgecolors="black",
    alpha=0.9,
)
plt.colorbar(scatter, label="Promedio de Frames por Extracción")
for _, row in top4.drop_duplicates(subset=["ID_Mina"]).iterrows():
  plt.annotate(
      f"ID {int(row['ID_Mina'])}",
      (row["Pos_X"], row["Pos_Y"]),
      textcoords="offset points",
      xytext=(0, 8),
      ha="center",
      fontsize=8,
      fontweight="bold",
  )

plt.gca().invert_yaxis()
plt.title(
    "Distribución Espacial de Eficiencia de Minerales en el Mapa",
    fontsize=13,
    fontweight="bold",
)
plt.xlabel("Coordenada X (píxeles)")
plt.ylabel("Coordenada Y (píxeles)")
plt.tight_layout()
plt.savefig("grafica_mapa_espacial.png", dpi=300)
plt.close()

print(
    "\nArchivos generados: 'tabla_top4_eficiencia.csv', 'resumen_comparativo_razas.csv' y 3 gráficas PNG."
)