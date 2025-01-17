#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_NAME 30
#define DEPARTAMENTOS 15

typedef struct DEPT {
	char desig[20];
	double ganhos, despesas;
	int n_pessoas;
};
typedef struct BTREE_NODE {
	void* data;
	struct BTREE_NODE* left;
	struct BTREE_NODE* right;
};

// Macros

#define DATA(node) ((node)->data)
#define LEFT(node) ((node)->left)
#define RIGHT(node) ((node)->right)

// Vari�veis Globais
int workers = 0;

// Prot�tipos

BTREE_NODE* NewTreeNode(void* data); // Tenta alocar um novo n� com a data passada, retorna false se falhar
void BtreeFree(BTREE_NODE* root); // Desaloca uma �rvore inteira da mem�ria
BTREE_NODE* InitNode(void* data, BTREE_NODE* left, BTREE_NODE* right); // Inicializa um n� com os filhos e data passados
BTREE_NODE* CreateBtree(void** data, int i, int size); // Cria uma �rvore com a data carregada no apontador void**
bool ReadFile(char* fileName, void** data); // L� um ficheiro e carrega os dados para o ponteiro void**
double GetSingleDptProfit(BTREE_NODE* root); // Retorna um float que representa os lucros de um departamento (ganhos-despesas)
double GetCompanyProfits(BTREE_NODE* root); //Retorna um float que representa os lucros de todos os departamentos da empresa (ganhos-despesas)
int GetMostPeopleDepartment(BTREE_NODE* root); // Retorna o n�mero de Pessoas do departamento com maior n�mero de pessoas
void ShowMostPeopleDepartment(BTREE_NODE* root, int count); // Mostra as informa��es do(s) Departamento(s) com maior n� de pessoas
void ShowDepartmentInfo(BTREE_NODE* root); // Mostra as informa��es de um departamento
bool IsDepartment(BTREE_NODE* root, const char* name); // Verifica o n� passado � a diretoria passada
bool IsBtreeNode(BTREE_NODE* root, const char* name); // Verifica se um n� pertence a uma �rvore
BTREE_NODE* find(BTREE_NODE* root, char* name); // Procura um departamento atrav�s da designa��o e retorna o seu apontador
void ShowDepartment(BTREE_NODE* root); // Mostra no ecr� as informa��es do departamento passado
void ShowAllDepartments(BTREE_NODE* root); // Mostra todos os departamentos

void Menu(char* OP); // Menu de op��es
void Buffer(void); // Limpa o buffer

int main(void)
{
	char fileName[MAX_NAME] = "Departamentos.txt";
	char OP = ' ';
	void* Departamentos[DEPARTAMENTOS];
	int count = 0;
	char desig[MAX_NAME] = " ";
	BTREE_NODE* tree = NULL;
	BTREE_NODE* aux = NULL;

	printf("Tentando abrir o ficheiro %s...\n", fileName);
	Sleep(1000);

	// Tenta abrir o ficheiro

	if (!(ReadFile(fileName, Departamentos))) {
		printf("Impossivel abrir o ficheiro \"%s\"...\n", fileName);
		Sleep(1000);
		return 0;
	}
	printf("\"%s\" lido com sucesso!\n", fileName);
	Sleep(1000);
	if ((tree = CreateBtree(Departamentos, 0, DEPARTAMENTOS)) == NULL) {
		printf("Ocorreu um erro ao tentar carregar a informacao para uma arvore!\n");
		Sleep(1000);
		return 0;
	}
	Sleep(1000);
	system("CLS");
	// Apresenta��o do Menu
	do {
		Menu(&OP);
		Buffer();
		switch (OP) {
			// Sai do programa
		case '0':
			exit(0);
			break;
			// Mostra lucros da empresa
		case '1':
			printf("\n-------------------------\nMostar Lucros da Empresa\n-------------------------\n");
			printf("Total de Lucros: Aprox. %.2lf Euros", GetCompanyProfits(tree));
			break;
			// Mostrar departamento com maior n�mero de pessoas
		case '2':
			printf("\n---------------------------------------------------\nMostrar Departamento(s) com maior numero de Pessoas\n---------------------------------------------------\n");
			count = GetMostPeopleDepartment(tree);
			ShowMostPeopleDepartment(tree, count);
			break;
			//  Elimina todos os sub-departamentos de um departamento e transfere todos os trabalhadores para este
		case '3':
			printf("\n----------------------------------------------\nEliminar Sub-Departamentos de um Departamento:\n----------------------------------------------\n");
			printf("\nDesignacao do Departamento: ");
			scanf("%s", &desig);
			if (!IsBtreeNode(tree, desig))
				// Verifica se o departamento introduzido se encontra na �rvore
				printf("O Departamento \"%s\" nao existe!\n", desig);
			else {
				// Liberta as sub-�rvores da esquerda ou da direita deixando a ra�z intacta de modo a que esta "acolha" os trabalhadores dos filhos apagados
				/*BtreeFree(LEFT(find(tree, desig)));
				BtreeFree(RIGHT(find(tree, desig)));
				((DEPT*)DATA(find(tree, desig)))->n_pessoas += workers;*/
				BtreeFree(find(tree, desig));
				printf("\nSub-Departamentos eliminados com sucesso!\n");
				printf("\nNovas informacoes do Departamento \"%s\":\n\n", desig);
				ShowDepartment(find(tree, desig));
			}
			break;
			// Mostra Departamento
		case '4':
			printf("\n-------------------------\nInformacoes Departamento:\n-------------------------\n");
			printf("\nDesignacao do Departamento: ");
			scanf("%s", &desig);
			if (!IsBtreeNode(tree, desig))
				// Verifica se o departamento introduzido se encontra na �rvore
				printf("O Departamento \"%s\" nao existe!\n", desig);
			else {
				ShowDepartment(find(tree, desig));
				printf("\n------------------------------\nInformacoes Sub-Departamentos:\n------------------------------\n\n");
				ShowAllDepartments(LEFT(find(tree, desig)));
				ShowAllDepartments(RIGHT(find(tree, desig)));
			}
			break;
		case '5':
			printf("\n--------------------------\nInformacoes Departamentos:\n--------------------------\n");
			ShowAllDepartments(tree);
			break;
			// Op��o inv�lida
		default:
			printf("Opcao invalida!\n");
			break;
		}

		workers = 0;
		count = 0;
		strcpy(fileName, " ");
		strcpy(desig, " ");
		printf("\n");
		Buffer();
		system("pause");
		system("CLS");
	} while (OP != '0');
	return 0;
}

// Fun��es

BTREE_NODE* NewTreeNode(void* data) {
	BTREE_NODE* newNode = NULL;
	// Verifica se a data � v�lida
	if (data == NULL)
		return NULL;
	// Aloca��o din�mica
	if (newNode = (BTREE_NODE*)malloc(sizeof(BTREE_NODE))) {
		// Atribui��o de dados
		DATA(newNode) = data;
		LEFT(newNode) = NULL;
		RIGHT(newNode) = NULL;
	}
	// Retorno
	return newNode;
}
void BtreeFree(BTREE_NODE* root) {
	// Caso a root seja nula retorna para libertar o n� pai, se a root for nula desde vo in�cio, a �rvore est� vazia
	if (root == NULL)
		return;
	// Chamada recursiva para libertar primeiro todos os n�s que est�o abaixo do atual
	BtreeFree(LEFT(root));
	BtreeFree(RIGHT(root));
	if (LEFT(root) != NULL){
		((DEPT*)DATA(root))->n_pessoas += ((DEPT*)DATA(LEFT(root)))->n_pessoas;
		free(DATA(LEFT(root)));
		free (LEFT(root));
		LEFT(root) = NULL;
	}
	if (RIGHT(root) != NULL) {
		((DEPT*)DATA(root))->n_pessoas += ((DEPT*)DATA(RIGHT(root)))->n_pessoas;
		free(DATA(RIGHT(root)));
		free(RIGHT(root));
		RIGHT(root) = NULL;
	}

}
	
BTREE_NODE* InitNode(void* data, BTREE_NODE* left, BTREE_NODE* right) {
	BTREE_NODE* newNode = NULL;
	// Verifica se a data � v�lida
	if (data == NULL)
		return NULL;
	// Tenta inicializar o n�
	if (!(newNode = NewTreeNode(data)))
		return NULL;
	// Atribui os filhos
	LEFT(newNode) = left;
	RIGHT(newNode) = right;
	// Retorna
	return newNode;
}
BTREE_NODE* CreateBtree(void** data, int i, int size) {
	if (i >= size)
		return NULL;
	return(InitNode(*(data + i), CreateBtree(data, 2 * i + 1, size), CreateBtree(data, 2 * i + 2, size)));
}
bool ReadFile(char* fileName, void** data) {
	FILE* ptr = NULL;
	void* voidPTR = NULL;
	int i = 0;
	// Verifica se os dados s�o v�lidos
	if (fileName == NULL || data == NULL)
		return false;
	// Tenta abrir o ficheiro
	if (!(ptr=fopen(fileName,"r")))
		return false;
	// Carrega os dados para o ponteiro
	while (!feof(ptr)){
		// Tenta alocar din�micamente um ponteiro void* para carregar as informa��es no ponteiro void**
		if ((voidPTR = (DEPT*)malloc(sizeof(DEPT))) != NULL) {
			fscanf(ptr, "%[^;];%lf;%lf;%d\n", ((DEPT*)voidPTR)->desig, &(((DEPT*)voidPTR)->ganhos), &(((DEPT*)voidPTR)->despesas), &(((DEPT*)voidPTR)->n_pessoas));
			data[i++] = voidPTR;
		}
		// Caso esta aloca��o falhe, � necess�rio desalocar quaisquer dados que tenham sido pr�viamente alocados
		else {
			for (int j = i; j >= 0; j--)
				free(data[j]);
			return false;
		}
	}
	fclose(ptr);
	return true;
}
double GetSingleDptProfit(BTREE_NODE* root) {
	// Se a ra�z for nula, a �rvore est� vazia, ou seja, n�o h� nenhum departamento
	if (root == NULL)
		return 0;
	return ((((DEPT*)DATA(root))->ganhos) - (((DEPT*)DATA(root))->despesas));
}
double GetCompanyProfits(BTREE_NODE* root) {
	// Se a ra�z for nula, a �rvore est� vazia, ou seja, n�o h� nenhum departamento
	if (root == NULL)
		return 0;
	//return GetSingleDptProfit(LEFT(root)) + GetSingleDptProfit(RIGHT(root));
	return GetSingleDptProfit(root) + GetCompanyProfits(LEFT(root)) + GetCompanyProfits(RIGHT(root));

}
int GetMostPeopleDepartment(BTREE_NODE* root) {
	// Se a ra�z for nula n�o h� departamentos a procurar
	int maior = 0;
	int maiorLeft,maiorRight ;
	if (root == NULL)
		return maior;
	// Compara o maior n�mero de pessoas atual com o n�mero de pessoas do n� atual, se menor atualiza-o
	if ((((DEPT*)DATA(root))->n_pessoas) > maior)
		maior = (((DEPT*)DATA(root))->n_pessoas);
	// Chamada recursiva para as sub-�rvores da esquerda e direita
	maiorLeft = GetMostPeopleDepartment(LEFT(root));
	maiorRight = GetMostPeopleDepartment(RIGHT(root));
	
	if (maiorLeft > maior)
		maior = maiorLeft;
	if (maiorRight > maior)
		maior = maiorRight;
	return maior;
}
void ShowMostPeopleDepartment(BTREE_NODE* root, int count) {
	// Se a ra�z � nula, n�o h� departamentos
	if (root == NULL)
		return;
	// Compara o n�mero de pessoasa do departamento do n� atal com o maior n�mero de pessoas, se for igual mostra as informa��es do departamento
	if ((((DEPT*)DATA(root))->n_pessoas) == count)
		ShowDepartmentInfo(root);
	// Chamadas recursivas (Pode haver 2 departamentos com o maior n�mero de pessoas)
	ShowMostPeopleDepartment(LEFT(root), count);
	ShowMostPeopleDepartment(RIGHT(root), count);
	return;
}
void ShowDepartmentInfo(BTREE_NODE* root) {
	printf("\nNome: %s\nGanhos: %.2lf Euros\nDespesas: %.2lf Euros\nNumero de Pessoas: %d\n", ((DEPT*)DATA(root))->desig, ((DEPT*)DATA(root))->ganhos, ((DEPT*)DATA(root))->despesas, ((DEPT*)DATA(root))->n_pessoas);
}
bool IsDepartment(BTREE_NODE* root, const char* name) {
	// Verifica se os dados s�o v�lidos
	if (root == NULL || name == NULL)
		return false;
	// Verifica se o n� atual � o n� procurado
	if (!strcmp((((DEPT*)DATA(root))->desig), name))
		return true;
	return false;
}
bool IsBtreeNode(BTREE_NODE* root, const char* name) {
	// Verifica se os dados s�o v�lidos
	if (root == NULL || name == NULL)
		return false;
	// Verifica se o n� atual � o procurado
	if (IsDepartment(root, name))
		return true;

	// Chamada recursiva at� encontrar o n� procurado, (se n�o encontrar retorna false)
	return IsBtreeNode(LEFT(root), name) || IsBtreeNode(RIGHT(root), name);
}
BTREE_NODE* find(BTREE_NODE* root, char* name)
{
	// Se a ra�z for nula, n�o h� departamentos
	if (root == NULL)
		return NULL;
	// Caso o departamento seja o n� atual
	if (!strcmp(((DEPT*)DATA(root))->desig, name))
		return root;
	// Chamada recursiva para a sub-�rvore esquerda
	BTREE_NODE* left = find(root->left, name);
	if (left)
		return left;
	// Chamada recursiva para a sub-�rvore direita
	BTREE_NODE* right = find(root->right, name);
	if (right)
		return right;
	// N�o encontrou
	return NULL;
}
void ShowDepartment(BTREE_NODE* root) {
	if (root == NULL) {
		printf("Departamento nao existente!\n");
		return;
	}
	printf("Designacao: %s\nGanhos: %.2lf\nDespesas: %.2lf\nTrabalhadores: %d\n\n", ((DEPT*)DATA(root))->desig, ((DEPT*)DATA(root))->ganhos, ((DEPT*)DATA(root))->despesas, ((DEPT*)DATA(root))->n_pessoas);
}
void ShowAllDepartments(BTREE_NODE* root){
	if (root == NULL)
	return;

	ShowAllDepartments(LEFT(root));
	ShowAllDepartments(RIGHT(root));
	ShowDepartment(root);
}
void Menu(char* OP) {
	printf("--------------------\nSelecione uma opcao:\n--------------------\n");
	printf("[1] Mostrar lucros da Empresa\n");
	printf("[2] Mostrar dados do departamento com maior numero de pessoas\n");
	printf("[3] Eliminar todos os sub-departamentos de um departamento e transferir trabalhadores para este\n");
	printf("[4] Mostrar dados de um Departamento\n");
	printf("[5] Mostrar dados de todos os Departamentos\n");
	printf("Opcao: ");
	scanf("%c", OP);
}
void Buffer(void) {
	while (getchar() != '\n');
	fflush(stdin);
}