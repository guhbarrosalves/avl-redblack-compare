"""Gera os gráficos a partir de resultados/resultados.json.

Cada contador é dividido pelo número de operações da fase (custo médio por
operação). No padrão aleatório, a linha é a média entre as sementes e a faixa
sombreada é ±1 desvio padrão. Crescente e decrescente têm uma única execução.

Uso (a partir da raiz do repositório):
    python3 experimentos/plot_results.py
"""
import argparse
import json
import math
import os
import statistics

import matplotlib
matplotlib.use("Agg")            # só grava arquivos; não precisa de janela
import matplotlib.pyplot as plt

ENTRADA = "resultados/resultados.json"
PASTA_SAIDA = "resultados/graficos"

PADROES = ["crescente", "decrescente", "aleatorio"]
NOME_PADRAO = {"crescente": "Entrada crescente",
               "decrescente": "Entrada decrescente",
               "aleatorio": "Entrada aleatória"}
COR_PADRAO = {"crescente": "tab:green", "decrescente": "tab:purple", "aleatorio": "tab:orange"}
# Crescente e decrescente são espelhos e costumam dar valores quase iguais; por isso a crescente é
# desenhada grossa e translúcida e a decrescente tracejada por cima, para as duas ficarem visíveis.
ESTILO_PADRAO = {"crescente": {"linewidth": 4.5, "alpha": 0.5},
                 "decrescente": {"linewidth": 1.5, "linestyle": "--"},
                 "aleatorio": {"linewidth": 1.5}}

# Mesma cor e marcador de cada árvore em todas as figuras.
ARVORES = {"avl": {"nome": "AVL", "cor": "tab:blue", "marcador": "o"},
           "rb": {"nome": "Rubro-negra", "cor": "tab:red", "marcador": "s"}}

# Contador de "manutenção do balanceamento" de cada árvore (medem coisas diferentes).
MANUTENCAO = {"avl": ("up_height", "AVL: atualizações de altura (up_height)"),
              "rb": ("rec", "Rubro-negra: trocas de cor (rec)")}


# ----------------------------------------------------------------- dados

def por_operacao(fase, campo):
    """Valor do contador dividido pelo número de operações da fase (None se não houve operação)."""
    def extrair(r):
        operacoes = r["operacoes"][fase]
        return r["contadores"][fase][campo] / operacoes if operacoes else None
    return extrair


def altura(chave):
    return lambda r: r["altura"][chave]


def agregar(resultados, arvore, padrao, extrair):
    """Para cada N, média e desvio padrão entre as execuções (sementes)."""
    por_n = {}
    for r in resultados:
        if r["arvore"] == arvore and r["padrao"] == padrao:
            valor = extrair(r)
            if valor is not None:
                por_n.setdefault(r["n"], []).append(valor)
    ns = sorted(por_n)
    return ns, [statistics.mean(por_n[n]) for n in ns], [statistics.pstdev(por_n[n]) for n in ns]


# ------------------------------------------------------ limites teóricos

def limite_avl(n):
    """Altura máxima de uma AVL com n nós, contando níveis (Knuth)."""
    return 1.4405 * math.log2(n + 2) - 0.3277


def limite_rb(n):
    """Altura máxima de uma rubro-negra com n nós, contando níveis (CLRS: 2 lg(n+1) arestas, mais 1)."""
    return 2 * math.log2(n + 1) + 1


# -------------------------------------------------------------- desenho

def desenhar(ax, resultados, arvore, padrao, extrair, cor, marcador, rotulo, estilo=None):
    ns, medias, desvios = agregar(resultados, arvore, padrao, extrair)
    if not ns:
        return
    estilo = {"linewidth": 1.4, **(estilo or {})}
    ax.plot(ns, medias, marker=marcador, markersize=3.5, color=cor, label=rotulo, **estilo)
    if padrao == "aleatorio":
        ax.fill_between(ns, [max(0, m - d) for m, d in zip(medias, desvios)],
                        [m + d for m, d in zip(medias, desvios)], color=cor, alpha=0.2, linewidth=0)


def grade(resultados, titulo, linhas, arquivo, com_limites=False):
    """Figura com uma linha de painéis por item de `linhas` e uma coluna por padrão de entrada."""
    fig, eixos = plt.subplots(len(linhas), 3, figsize=(15, 3.7 * len(linhas)),
                              sharex=True, sharey="row", squeeze=False)
    xs = [10 ** (k / 20) for k in range(0, 101)]           # curva suave de 1 a 100.000
    for i, (rotulo_y, extrair) in enumerate(linhas):
        for j, padrao in enumerate(PADROES):
            ax = eixos[i][j]
            for arvore, est in ARVORES.items():
                desenhar(ax, resultados, arvore, padrao, extrair, est["cor"], est["marcador"],
                         est["nome"] if (i == 0 and j == 0) else None)
            if com_limites:
                for limite, cor, nome in ((limite_avl, ARVORES["avl"]["cor"], "limite teórico AVL"),
                                          (limite_rb, ARVORES["rb"]["cor"], "limite teórico rubro-negra")):
                    ax.plot(xs, [limite(x) for x in xs], linestyle="--", linewidth=1, color=cor, alpha=0.6,
                            label=nome if (i == 0 and j == 0) else None)
            ax.set_xscale("log")
            ax.grid(True, alpha=0.3)
            if i == 0:
                ax.set_title(NOME_PADRAO[padrao])
            if j == 0:
                ax.set_ylabel(rotulo_y)
            if i == len(linhas) - 1:
                ax.set_xlabel("N (número de chaves, escala logarítmica)")
    eixos[0][0].legend(fontsize=8)
    fig.suptitle(titulo, fontsize=14)
    fig.tight_layout(rect=[0, 0, 1, 0.97])
    fig.savefig(arquivo, dpi=150)
    plt.close(fig)
    print("gerado:", arquivo)


def figura_manutencao(resultados, arquivo):
    """Manutenção do balanceamento: um painel por árvore, porque os contadores medem coisas diferentes."""
    fases = [("Inserção", "insercao"), ("Remoção (1ª metade)", "remocao_1a_metade")]
    fig, eixos = plt.subplots(2, 2, figsize=(11, 8), sharex=True, squeeze=False)
    for i, (rotulo_fase, fase) in enumerate(fases):
        for j, (arvore, (campo, titulo)) in enumerate(MANUTENCAO.items()):
            ax = eixos[i][j]
            for padrao in PADROES:
                desenhar(ax, resultados, arvore, padrao, por_operacao(fase, campo), COR_PADRAO[padrao], "o",
                         NOME_PADRAO[padrao] if (i == 0 and j == 0) else None, ESTILO_PADRAO[padrao])
            ax.set_xscale("log")
            ax.grid(True, alpha=0.3)
            if i == 0:
                ax.set_title(titulo)
            if j == 0:
                ax.set_ylabel(f"{rotulo_fase}\npor operação")
            if i == 1:
                ax.set_xlabel("N (número de chaves, escala logarítmica)")
    eixos[0][0].legend(fontsize=8)
    fig.suptitle("Manutenção do balanceamento (as escalas verticais dos dois lados são diferentes)", fontsize=13)
    fig.tight_layout(rect=[0, 0, 1, 0.96])
    fig.savefig(arquivo, dpi=150)
    plt.close(fig)
    print("gerado:", arquivo)


def main():
    ap = argparse.ArgumentParser(description="Gera os gráficos do experimento.")
    ap.add_argument("--entrada", default=ENTRADA)
    ap.add_argument("--saida", default=PASTA_SAIDA, help="pasta onde as imagens serão gravadas")
    args = ap.parse_args()

    try:
        with open(args.entrada) as f:
            resultados = json.load(f)["resultados"]
    except FileNotFoundError:
        raise SystemExit(f"Arquivo '{args.entrada}' não encontrado. Rode antes o run_all.py.")
    os.makedirs(args.saida, exist_ok=True)
    caminho = lambda nome: os.path.join(args.saida, nome)

    grade(resultados, "Altura da árvore (número de níveis)",
          [("Após a inserção\naltura", altura("apos_insercao")),
           ("Após remover metade das chaves\naltura", altura("apos_metade_das_remocoes"))],
          caminho("altura.png"), com_limites=True)
    grade(resultados, "Rotações por operação",
          [("Inserção\nrotações por operação", por_operacao("insercao", "rot")),
           ("Remoção (1ª metade)\nrotações por operação", por_operacao("remocao_1a_metade", "rot"))],
          caminho("rotacoes.png"))
    grade(resultados, "Comparações de chaves por operação",
          [("Inserção\ncomparações por operação", por_operacao("insercao", "compare")),
           ("Busca de existentes\ncomparações por operação", por_operacao("busca_existente", "compare")),
           ("Busca de inexistentes\ncomparações por operação", por_operacao("busca_inexistente", "compare")),
           ("Remoção (1ª metade)\ncomparações por operação", por_operacao("remocao_1a_metade", "compare"))],
          caminho("comparacoes.png"))
    figura_manutencao(resultados, caminho("manutencao.png"))


if __name__ == "__main__":
    main()