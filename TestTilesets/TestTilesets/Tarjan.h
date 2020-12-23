#ifndef TARJAN
#define TARJAN

#include "Tileset.h"

const int UNDEFINED_INDEX = -1;

//���������, ������� �������� ���, ��� ����� ��� ��������� �������
struct Tarjan
{
	//���������� ������ � �����
	int size;
	//���������� ������������� ���������
	int sccSize;

	//����, � ������� �������� �������� �� ������� ������ ������� ����������
	std::vector<Color> stack;
	//index - ��� "�������" ������ (������ ���������)
	//lowlink - ��� ���������� ������ ������� � ��������������� ����������, � ������� ����������� ������ �������
	//������ ������ ����� ����� - ���-�� ������. �������������� i-� ������� ��������� i-� �������
	std::vector<int> index, lowlink;
	//������ ������������� ����������, � ������� ��������� ��� �������
	std::vector<int> sccIndexes;
	//����� �������� ������: ��� ������ - ��� ���-�� ������, � �������� - ��� ������ ������ � ������ �����, 
	//� �������� ���������� �����, ��������� �� ���� �������. � ����� ������ ����� - ��� �����, � ������� - �����
	std::vector<int> colorBeginningIndexes;
	//������, ������� �������� �� ��, ��������� �� ������� � �����
	std::vector<bool> onStack;
};

//�������� ������� ��������� �������. ������� ������� �����:
//https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
//������� ������ ���� ������ � ��������� ������
//��� ���� ���� ��� � ���������� �������. ������ ������� ������������ � ����������
//t - ���������, �������� ������ ����, tileset - �������, vertex - ����, ������� ������ ������������
//������ ������� �������� ������� �������� ������... �, �������, ����� ���, �� ����������� ��� ��� ����� �� ����
//������ ������� - �������� ���������, ��� ��� � �������� �� ��������
void ApplyTarjan(Tarjan& t, const Tileset tileset, const Color vertex) {
	//���� ����� ������� ������
	t.index[vertex] = t.size;
	//���� ��� �� ��������� ���� � ������������� ����������, � ������� ������ ��
	//������� lowlink ����� index-�
	t.lowlink[vertex] = t.index[vertex];
	t.size++;

	//��������� � ����
	t.stack.push_back(vertex);
	t.onStack[vertex] = true;

	//���� �� ���� ��������, ���������� �� ���� ��� �� ���� �������
	for (int i = t.colorBeginningIndexes[vertex]; ((i < tileset.size) && (tileset[i].west() == vertex)); i++)
	{
		//�������� ��� �������
		Color east = tileset[i].east();

		//���� �� � ��� ��� ������� �� ��������, �� ������������ �� ����������
		if (t.index[east] == UNDEFINED_INDEX) {
			//��������� ��� �� �������
			ApplyTarjan(t, tileset, east);
			//���� �� ������� east ����� ����� �� �����-�� �������, �� ������� �� ������ � ������� vertex,
			//������, vertex ����� � ����� ������� ������������� ����������. ������������� lowlink
			t.lowlink[vertex] = std::min(t.lowlink[vertex], t.lowlink[east]);
		}
		else {
			//���� �� ��� ��������� ��� �������, �� ��� ��� �� ���� �� �����
			//������, �� ������ � vertex �� east. ����� ��� � ����� ������������� ����������
			if (t.onStack[east]) {
				t.lowlink[vertex] = std::min(t.lowlink[vertex], t.index[east]);
			}
			//����� �� ��� ������� east � ������ ������������� ����������, ������� ��� ��������� 
			//� vertex ����� � ��� �� ���������, ��������� ��� ��� ����
		}
	}



	//���� vertex ����������� ������ ����� ���������� ������������� ���������� (������ ���� �� ������ �� �������)
	//�� ������� ���� � ���������� ��� ������������� ���������� (���� �� ������ � ���������� � ������ sccIndexes)
	if (t.index[vertex] == t.lowlink[vertex]) {
		Color w = -1;
		do {
			w = t.stack.back();
			t.stack.pop_back();

			t.onStack[w] = false;
			t.sccIndexes[w] = t.sccSize;
		} while (w != vertex);

		t.sccSize++;
	}
}

//������� ��������� �������, ������� ������ ������� ����� ����� �������������� ������������ � �������� ����� ������� �������
void ApplyTarjan(Tileset T) {
	if (T.size == 0) {
		return;
	}

	//���������, ����� ��� �����, � ������� north ���� ����������, ���� �����
	T.Sort();

	//�������������� ��������� ��� ��������� �������
	Tarjan t;
	t.size = 1;
	t.sccSize = 1;

	t.onStack = std::vector<bool>();
	t.onStack.resize(T.maxColor, false);

	t.index = std::vector<int>();
	t.index.resize(T.maxColor, UNDEFINED_INDEX);

	t.lowlink = std::vector<int>();
	t.lowlink.resize(T.maxColor, UNDEFINED_INDEX);

	t.sccIndexes = std::vector<int>();
	t.sccIndexes.resize(T.maxColor, UNDEFINED_INDEX);

	t.stack = std::vector<Color>();

	t.colorBeginningIndexes = std::vector<int>();
	t.colorBeginningIndexes.resize(T.maxColor);

	//����, � ������ ����� ���������� ����� ����
	Color currentColor = -1;
	for (int i = 0; i < T.size; i++)
	{
		if (currentColor != T[i].north()) {
			currentColor = T[i].north();
			t.colorBeginningIndexes[currentColor] = i;
		}
	}

	//��������� �������� �������
	for (int i = 0; i < T.maxColor; i++)
	{
		if (t.index[i] == UNDEFINED_INDEX) {
			ApplyTarjan(t, T, i);
		}
	}

	//���������� �� ���� ������ � ������� ��, � ������� ����� �� ������ ������������� ���������
	//��� ����� ��� �� ����� ������ �������� ������ ������, � ����� ������ ������ ������
	int newSize = 0;
	for (int i = 0; i < T.size; i++)
	{
		Color north = T[i].north();
		Color south = T[i].south();
		if (t.sccIndexes[north] == t.sccIndexes[south]) {
			if (newSize != i) {
				T.tiles[newSize] = T.tiles[i];
				newSize++;
			}
		}
	}

	T.size = newSize;
}

#endif