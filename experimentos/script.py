"""Executa UMA combinação do experimento: (árvore, padrão, N, semente).

O programa em C++ cria uma única árvore por execução. Este módulo:
  1. monta o texto dos comandos (inserções, buscas, remoções, COUNTERS...);
  2. entrega esse texto ao programa pela entrada padrão;
  3. lê a saída do programa e confere se tudo ocorreu como esperado;
  4. devolve os números em um dicionário (que depois vira JSON).

Uso (a partir da raiz do repositório, com o programa já compilado em ./tree):
    python3 experimentos/experimento.py avl aleatorio 1000 --semente 3
"""
import argparse
import json
import random
import re
import subprocess

EXECUTAVEL = "./tree"
PADROES = ["crescente", "decrescente", "aleatorio"]

# Mensagens que o programa imprime para cada operação (função Execute do
# main.cpp). A comparação ignora o número do começo e o ponto do fim, então
# "10 inserted." e "inserted" valem igual. Se mudar o texto no main.cpp,
# mude aqui também.
MSG_INSERIDO = "inserted"
MSG_REMOVIDO = "removed"
MSG_ENCONTRADO = "found"
MSG_NAO_ENCONTRADO = "not found"

# Linha de contadores: "COUNTERS nome=valor nome=valor ..." (tudo em uma linha).
# Exigir os pares nome=valor evita confundir com a linha de ajuda do programa
# ("COUNTERS - displays the counters"), que também começa com COUNTERS.
LINHA_COUNTERS = re.compile(r"COUNTERS((?:\s+\w+=\d+)+)\s*")


def gerar_chaves(n, padrao, semente):
    """N chaves pares (2, 4, ..., 2N), na ordem de inserção do padrão."""
    if padrao not in PADROES:
        raise ValueError(f"padrão desconhecido: {padrao}")
    chaves = [2 * i for i in range(1, n + 1)]
    if padrao == "decrescente":
        chaves.reverse()
    elif padrao == "aleatorio":
        random.Random(semente).shuffle(chaves)
    return chaves


def montar_comandos(arvore, n, padrao, semente):
    """Texto com todos os comandos de uma execução, fase por fase."""
    chaves = gerar_chaves(n, padrao, semente)
    # Os ímpares nunca são inseridos: servem para a busca de inexistentes e
    # ficam espalhados entre as chaves pares (um em cada "buraco" da árvore).
    inexistentes = [2 * i - 1 for i in range(1, n + 1)]
    # Ordem de remoção: sempre embaralhada, igual nas duas árvores.
    remocao = chaves[:]
    random.Random(semente + 1000).shuffle(remocao)
    metade = n // 2

    cmd = [arvore]                                   # primeira linha: tipo da árvore
    cmd += [f"INSERT {k}" for k in chaves]           # fase 1: inserção
    cmd += ["COUNTERS", "HEIGHT", "RESET"]
    cmd += [f"SEARCH {k}" for k in chaves]           # fase 2: busca de existentes
    cmd += ["COUNTERS", "RESET"]
    cmd += [f"SEARCH {k}" for k in inexistentes]     # fase 3: busca de inexistentes
    cmd += ["COUNTERS", "RESET"]
    cmd += [f"REMOVE {k}" for k in remocao[:metade]]  # fase 4: 1ª metade das remoções
    cmd += ["COUNTERS", "HEIGHT"]                    # (sem RESET: os contadores acumulam)
    cmd += [f"REMOVE {k}" for k in remocao[metade:]]  # 2ª metade das remoções
    cmd += ["COUNTERS", "END"]
    return "\n".join(cmd) + "\n"


def interpretar(saida):
    """Separa da saída as linhas COUNTERS e as linhas Height."""
    linhas = saida.splitlines()
    contadores = []
    for linha in linhas:
        achou = LINHA_COUNTERS.fullmatch(linha)
        if achou:
            contadores.append({nome: int(valor)
                               for nome, valor in re.findall(r"(\w+)=(\d+)", achou.group(1))})
    alturas = [int(linha.split()[1]) for linha in linhas if linha.startswith("Height:")]
    return linhas, contadores, alturas


def texto_da_mensagem(linha):
    """Tira o número do começo e o ponto do fim: '10 not found.' -> 'not found'."""
    return re.sub(r"^\d+\s*", "", linha.strip()).rstrip(".")


def conferir(linhas, n):
    """Garante que todas as operações tiveram o resultado esperado."""
    textos = [texto_da_mensagem(l) for l in linhas]
    esperado = {
        "inseridos": (textos.count(MSG_INSERIDO), n),
        "removidos": (textos.count(MSG_REMOVIDO), n),
        "encontrados": (textos.count(MSG_ENCONTRADO), n),
        "nao_encontrados": (textos.count(MSG_NAO_ENCONTRADO), n),
    }
    for nome, (obtido, certo) in esperado.items():
        if obtido != certo:
            raise RuntimeError(f"conferência falhou: {nome} = {obtido}, esperado {certo}")


def rodar_combinacao(arvore, n, padrao, semente=0, executavel=EXECUTAVEL):
    """Roda uma combinação e devolve os resultados (contadores brutos, sem dividir por N)."""
    comandos = montar_comandos(arvore, n, padrao, semente)
    r = subprocess.run([executavel], input=comandos, capture_output=True, text=True, check=True)
    linhas, c, h = interpretar(r.stdout)
    if len(c) != 5 or len(h) != 2:
        raise RuntimeError(
            f"saída inesperada: {len(c)} linhas COUNTERS (esperado 5) e {len(h)} linhas Height (esperado 2). "
            "Confira se o programa imprime os contadores em UMA linha, no formato "
            "'COUNTERS compare=1 rot=0 ...', e se as demais respostas terminam com quebra de linha.")
    conferir(linhas, n)

    metade = n // 2
    # c[3] foi lido após a 1ª metade; c[4] é o acumulado após todas as remoções.
    segunda_metade = {nome: c[4][nome] - c[3][nome] for nome in c[4]}
    return {
        "arvore": arvore,
        "padrao": padrao,
        "n": n,
        "semente": semente,
        # número de operações de cada fase (os gráficos dividem os contadores por isto)
        "operacoes": {
            "insercao": n,
            "busca_existente": n,
            "busca_inexistente": n,
            "remocao_1a_metade": metade,
            "remocao_2a_metade": n - metade,
        },
        "contadores": {
            "insercao": c[0],
            "busca_existente": c[1],
            "busca_inexistente": c[2],
            "remocao_1a_metade": c[3],
            "remocao_2a_metade": segunda_metade,
        },
        "altura": {
            "apos_insercao": h[0],
            "apos_metade_das_remocoes": h[1],
        },
    }


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description="Roda uma combinação do experimento e mostra o resultado.")
    ap.add_argument("arvore", choices=["avl", "rb"])
    ap.add_argument("padrao", choices=PADROES)
    ap.add_argument("n", type=int, help="número de chaves inseridas")
    ap.add_argument("--semente", type=int, default=0, help="semente (padrão aleatório e ordem de remoção)")
    ap.add_argument("--exe", default=EXECUTAVEL, help="caminho do programa compilado")
    args = ap.parse_args()
    try:
        resultado = rodar_combinacao(args.arvore, args.n, args.padrao, args.semente, args.exe)
    except FileNotFoundError:
        raise SystemExit(f"Programa '{args.exe}' não encontrado. Compile antes (g++ ... -o tree).")
    print(json.dumps(resultado, indent=2))