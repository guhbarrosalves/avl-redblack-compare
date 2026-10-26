"""Percorre todas as combinações do experimento e grava os resultados em JSON.

Para cada tamanho N, roda as duas árvores com as MESMAS chaves:
  - crescente e decrescente: 1 execução cada (não há aleatoriedade);
  - aleatório: uma execução por semente (as sementes variam a ordem das chaves).

Uso (a partir da raiz do repositório, com o programa já compilado em ./tree):
    python3 experimentos/run_all.py                  # experimento completo
    python3 experimentos/run_all.py --tamanhos 1 2 5 --sementes 2 --saida /tmp/teste.json   # teste rápido
"""
import argparse
import json
import os
import time

from script import EXECUTAVEL, PADROES, rodar_combinacao

# Tamanhos em passos geométricos: ficam igualmente espaçados em escala logarítmica.
TAMANHOS = [1, 2, 5, 10, 20, 50, 100, 200, 500,
            1000, 2000, 5000, 10000, 20000, 50000, 100000]
SEMENTES = 30                      # execuções do padrão aleatório (sementes 1 a 30)
ARVORES = ["avl", "rb"]
SAIDA = "resultados/resultados.json"


def sementes_do_padrao(padrao, n_sementes):
    """Crescente e decrescente não têm aleatoriedade: uma execução basta."""
    return range(1, n_sementes + 1) if padrao == "aleatorio" else [0]


def salvar(caminho, configuracao, resultados):
    pasta = os.path.dirname(caminho)
    if pasta:
        os.makedirs(pasta, exist_ok=True)
    with open(caminho, "w") as f:
        json.dump({"configuracao": configuracao, "resultados": resultados}, f, indent=1)


def main():
    ap = argparse.ArgumentParser(description="Roda todas as combinações do experimento.")
    ap.add_argument("--tamanhos", type=int, nargs="+", default=TAMANHOS)
    ap.add_argument("--sementes", type=int, default=SEMENTES)
    ap.add_argument("--saida", default=SAIDA)
    ap.add_argument("--exe", default=EXECUTAVEL)
    args = ap.parse_args()

    configuracao = {"tamanhos": args.tamanhos, "sementes_aleatorio": args.sementes,
                    "arvores": ARVORES, "padroes": PADROES}
    resultados = []
    inicio = time.perf_counter()

    for n in args.tamanhos:
        t0 = time.perf_counter()
        for padrao in PADROES:
            for semente in sementes_do_padrao(padrao, args.sementes):
                for arvore in ARVORES:       # mesma semente => mesmas chaves nas duas árvores
                    try:
                        resultados.append(rodar_combinacao(arvore, n, padrao, semente, args.exe))
                    except FileNotFoundError:
                        raise SystemExit(f"Programa '{args.exe}' não encontrado. Compile antes (g++ ... -o tree).")
                    except Exception as erro:
                        raise RuntimeError(f"falhou em {arvore} {padrao} N={n} semente={semente}: {erro}") from erro
        salvar(args.saida, configuracao, resultados)     # grava a cada tamanho: nada se perde se parar no meio
        print(f"N={n:>6}: ok ({time.perf_counter() - t0:5.1f} s) | {len(resultados)} resultados até agora")

    print(f"\nPronto: {len(resultados)} resultados em {args.saida} ({time.perf_counter() - inicio:.0f} s no total)")


if __name__ == "__main__":
    main()