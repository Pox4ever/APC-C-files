// Carlos Andre Velozo Trufini
// Matricula: 1222130030

#include "leitura.h"

#define N 50

typedef struct Produto {
    int codigo;
    char descricao[100];
    float valor_unitario;
    int qtd_estoque;
} Produto;

int cadastrar(Produto p[], int posicao, Produto produto) {
    p[posicao] = produto;
    return ++posicao;
}

void listar(Produto p[], int nProdutos) {
    int i;
    for(i = 0;i < nProdutos;i++) {
        if (p[i].codigo != 0) {
            printf("\nCodigo: %i. \n",p[i].codigo);
            printf("Descricao: %s\n",p[i].descricao);
            printf("Valor unitario: %.2f. \n",p[i].valor_unitario);
            printf("Quantidade em estoque: %i. \n\n\n",p[i].qtd_estoque);
        }
    system("PAUSE");
    }
}

int localizar(Produto p[], int codigo, int quant) {
    int i;
    for(i = 0;i < quant;i++) {
        if (p[i].codigo == codigo)
            {
            return i;
            }
    }
    return -1;
}

void sobreregistro (Produto p[], int codigo) {
    int i;
    for (i=0;i < N;i++)
    {
        if (p[i].codigo == codigo)
        {
            printf("\nCodigo: %i. \n",p[i].codigo);
            printf("Descricao: %s\n",p[i].descricao);
            printf("Valor unitario: %.2f. \n",p[i].valor_unitario);
            printf("Quantidade em estoque: %i. \n\n\n",p[i].qtd_estoque);
        }
    }
    system("PAUSE");
}

float alterarvalor(Produto p[], int codigo, float novovalor, int quant) {
    int i = localizar(p,codigo,quant);
    if (i == -1) //not found
        return 0;
    else
        p[i].valor_unitario = novovalor;
}

int resetarvalores(Produto p[], int codigo, int quant) {
    int i = localizar(p,codigo,quant);
    if (i == -1)
        return 0;
    else {
        p[i].codigo = 0;
        strcpy(p[i].descricao,"");
        p[i].qtd_estoque = 0;
        p[i].valor_unitario = 0.0;
        return 1;
    }
}

float somapreco(Produto p[],int nProdutos) {
    int i;
    float precos;
    precos = 0;
    for (i=0;i < nProdutos;i++)
    {
        precos = p[i].valor_unitario + precos;
    }
    return precos;
}


int contaRegistros(Produto p[],int quant) {
    int i,cont = 0;
    for (i = 0;i < quant;i++) {
        if (p[i].codigo != 0)
            cont++;
    }
    return cont;
}

float mediapreco(Produto p[],int nProdutos) {
    float s = somapreco(p,nProdutos);
    int n = contaRegistros(p,nProdutos);
    return s/n;
}

float produtocaro(Produto p[],int nProdutos) {
    int i;
    float maior;
    maior = p[0].valor_unitario;
    for (i=0;i < nProdutos;i++)
    {
        if (p[i].valor_unitario > maior && p[i].codigo != 0)
        {
            maior = p[i].valor_unitario;
        }
    }
    return maior;
}

float produtobarato(Produto p[],int nProdutos) {
    int i;
    float menor;
    menor = p[0].valor_unitario;
    for (i=0;i < nProdutos;i++)
    {
        if (p[i].valor_unitario < menor && p[9].codigo != 0)
        {
            menor = p[i].valor_unitario;
        }
    }
    return menor;
}

void crescentecodigo (Produto p[], int nProdutos) {
    int i,j;
    Produto aux;
    for(i = nProdutos-1;i > 0; i--)
    {
        for(j=0;j<1;j++)
        {
            if(p[j].codigo>p[j+1].codigo)
            {
                aux=p[j];                        // ##########BOLHA
                p[j]=p[j+1];
                p[j+1]=aux;
            }
        }
    }
}

void crescentedescricao (Produto p[], int nProdutos) {
    int i,j;
    Produto temp;
    for (i = nProdutos-1;i > 0;i--)
    {
        for (j=0;j<1;j++) {
            if (strcmp(p[j].descricao,p[j+1].descricao) > 0) {
                temp = p[j];
                p[j] = p[j+1];
                p[j+1] = temp;
            }
        }
    }
}

int saveFile(Produto p[],int quant) {
    FILE *arq = fopen("arquivo.bin","wb");
    if (arq != NULL) {
        fwrite(p,sizeof(Produto),quant,arq);
        fclose(arq);
        return 1;
        }
    else
        return 0;
}

int openFile (Produto p[]) {
    FILE *arq = fopen ("arquivo.bin","rb");
    int i = 0;
    while(arq != NULL && !feof(arq)) {
        if (fread(&p[i],sizeof(Produto),1,arq) == 1)
        i++;
    }
    if (arq != NULL) {
        fclose(arq);
        }
    return i;
}

void main () {
    Produto produtos[N];
    int nCadastrados = 0;
    int opcao;
    int codigo, indice;
    char save;
    float novovalor;
    Produto p;
    printf("\nCarregando arquivo.\n");
    nCadastrados = openFile(produtos);
    if (nCadastrados > 0)
        printf("Arquivo carregado com sucesso.\n");
    else
        printf("Nao existe um arquivo para ser carregado ainda.\n");
    do {
        printf("MENU PRINCIPAL\n");
        printf("1 - Cadastrar novo produto.\n");
        printf("2 - Listar produtos cadastrados.\n");
        printf("3 - Localizar um registro.\n");
        printf("4 - Exibir informacoes sobre o registro.\n");
        printf("5 - Alterar valor unitario.\n");
        printf("6 - Resetar registro de um produto. (Remover todas as informacoes)\n");
        printf("7 - Exibir a soma do preco de todos os produtos.\n");
        printf("8 - Media dos precos de todos os produtos.\n");
        printf("9 - Exibir o valor do produto mais caro.\n");
        printf("10 - Exibir o valor do produto mais barato.\n");
        printf("11 - Ordenar registros em ordem crescente (codigo)\n");
        printf("12 - Ordenar registros em ordem crescente (descricao)\n");
        printf("13 - Salvar arquivo.\n");
        printf("99 - SAIR.\n");
        printf("! Digite uma opcao acima ! : ");
        opcao = lerInteiro();
        switch (opcao) {
            case 1: //cadastrar
                printf("Cadastro de Produtos\n");
                printf("\nDigite o codigo do produto: ");
                p.codigo = lerInteiro();
                if (p.codigo <= 0)
                    {
                        printf("Codigo invalido !\n");
                    }
                else {
                printf("Digite a descricao do produto: ");
                lerString(p.descricao,100);
                printf("Digite o valor unitario do produto: ");
                p.valor_unitario = lerReal();
                printf("Digite a quantidade em estoque do produto: ");
                p.qtd_estoque = lerInteiro();
                nCadastrados = cadastrar(produtos,nCadastrados,p);
                }                                                        //Vetor,posi��o,produto.
                break;
            case 2: // Listar
                listar(produtos,nCadastrados);
                break;
            case 3: //Localizar
                printf("Localizacao do produto.\n");
                printf("\n\nDigite o codigo do produto para localiza-lo: ");
                codigo = lerInteiro();
                indice = localizar(produtos, codigo, nCadastrados);
                if (indice == -1)
                    printf("Produto de codigo %i nao cadastrado.\n",codigo);
                else
                {
                    printf("Produto de codigo %i encontrado na posicao %i.\n", codigo, indice);
                    printf("Descricao: %s",produtos[indice].descricao);
                    printf("Valor unitario: %.2f. \n",produtos[indice].valor_unitario);
                    printf("Quantidade em estoque: %i. \n\n\n",produtos[indice].qtd_estoque);
                }
                system("PAUSE");
                break;
            case 4: //Exibir informa��es de um codigo de produto espec�fico.
                printf("Exibir informacoes de um codigo de produto especifico.\n");
                printf("\n\nDigite o codigo do produto para exibir as informacoes: ");
                codigo = lerInteiro();
                sobreregistro(produtos, codigo);
                break;
            case 5: //Alterar valor unitario.
                printf("Alterar valor unitario.\n");
                printf("Digite o codigo do produto para alterar o valor: ");
                codigo = lerInteiro();
                printf("\nDigite o novo valor do produto: ");
                novovalor = lerReal();
                if (alterarvalor(produtos,codigo,novovalor,nCadastrados))
                    printf("Valor alterado com sucesso.\n");
                else
                    printf("Produto n�o cadastrado.\n");
                break;
            case 6: //Resetar informacoes.
                printf("Resetar informacoes de um produto.\n");
                printf("\nDigite o codigo do produto para resetar as informacoes: ");
                codigo = lerInteiro();
                if (resetarvalores(produtos, codigo, nCadastrados))
                    printf("Produto removido com sucesso!\n");
                else
                    printf("Produto nao cadastrado!\n");
                break;
            case 7: //Soma dos pre�os
                printf("\n\nA soma dos precos de todos os produtos eh: %.2f.\n\n",somapreco(produtos,nCadastrados));
                system("PAUSE");
                break;
            case 8: //Media dos pre�os
                printf("\n\nA media dos precos dos produtos eh: %.2f.\n\n",mediapreco(produtos,nCadastrados));
                system("PAUSE");
                break;
            case 9: //Produto mais caro.
                printf("\n\nO produto mais caro custa: %.2f.\n\n",produtocaro(produtos,nCadastrados));
                system("PAUSE");
                break;
            case 10: //Produto mais barato
                printf("\nO produto mais barato custa: %.2f.\n\n",produtobarato(produtos,nCadastrados));
                system("PAUSE");
                break;
            case 11: //Ordem crescente c�digo
                crescentecodigo(produtos,nCadastrados);
                printf("\n\nA ordem dos registros foram alteradas para uma ordem crescente em relacao ao numero do codigo.\n\n");
                system("PAUSE");
                break;
            case 12: //Ordem crescente descri��o
                crescentedescricao(produtos,nCadastrados);
                printf("\n\nA ordem dos registros foram alteradas para uma ordem crescente em relacao ao numero de caracteres da descricao.\n\n");
                system("PAUSE");
                break;
            case 13: //Salvar arquivo
                printf("\nSalvar arquivo.\n");
                if (saveFile(produtos,nCadastrados))
                {
                    printf("\nArquivo salvo com sucesso!\n");
                }
                else
                    {}
                break;
            case 99: //Sair, salvar ?
                printf("Deseja salvar o arquivo antes de sair ? (S/N): ");
                save = lerCaractere();
                if (save == 's' || save == 'S') {
                    saveFile(produtos,nCadastrados);
                    printf("\nArquivo salvo com sucesso.\n");
                    break;
                }
                else
                    printf("\nArquivo nao salvo, saindo do programa.. .\n");
                    break;
            default:;
        }
    } while (opcao != 99);
}