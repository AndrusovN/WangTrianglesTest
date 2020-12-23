#ifndef TARJAN
#define TARJAN

#include "Tileset.h"

const int UNDEFINED_INDEX = -1;

//Структура, которая содержит все, что нужно для алгоритма Тарьяна
struct Tarjan
{
	//Количество вершин в графе
	int size;
	//Количество сильносвязных компонент
	int sccSize;

	//Стек, в котором хранятся элементы из текущей сильно связной компоненты
	std::vector<Color> stack;
	//index - это "индексы" вершин (просто нумерация)
	//lowlink - это наименьший индекс вершины в сильносвязанной компоненте, к которой принадлежит данная вершина
	//каждый список имеет длину - кол-во вершин. Соответственно i-й элемент описывает i-ю вершину
	std::vector<int> index, lowlink;
	//индекс сильносвязной компоненты, к которой относится эта вершина
	std::vector<int> sccIndexes;
	//очень полезный массив: его размер - это кол-во вершин, а значение - это первый индекс в списке ребер, 
	//с которого начинаются ребра, исходящие из этой вершины. В нашем случае ребра - это тайлы, а вершины - цвета
	std::vector<int> colorBeginningIndexes;
	//массив, который отвечает за то, находится ли вершина в стеке
	std::vector<bool> onStack;
};

//Основная функция алгоритма Тарьяна. Неплохо описана здесь:
//https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
//Русская версия этой статьи в википедии ужасна
//Это лишь один шаг в выполнении функции. Полная функция представлена в перегрузке
//t - структура, хранящая нужную инфу, tileset - тайлсет, vertex - цвет, который сейчас обрабатываем
//Вообще описать алгоритм Тарьяна довольно сложно... Я, кажется, понял его, но пересказать так вот сходу не могу
//Лучшее решение - смотреть википедию, мой код и надеятся на озарение
void ApplyTarjan(Tarjan& t, const Tileset tileset, const Color vertex) {
	//Даем новой вершине индекс
	t.index[vertex] = t.size;
	//Пока что он относится лишь к сильносвязной компоненте, в которой только он
	//Поэтому lowlink равен index-у
	t.lowlink[vertex] = t.index[vertex];
	t.size++;

	//Добавляем в стек
	t.stack.push_back(vertex);
	t.onStack[vertex] = true;

	//Идем по всем вершинам, достижимым за один шаг из этой вершины
	for (int i = t.colorBeginningIndexes[vertex]; ((i < tileset.size) && (tileset[i].west() == vertex)); i++)
	{
		//Получаем эту вершину
		Color east = tileset[i].east();

		//Если мы с ней еще никогда не работали, то обрабатываем ее рекурсивно
		if (t.index[east] == UNDEFINED_INDEX) {
			//Применяем эту же функцию
			ApplyTarjan(t, tileset, east);
			//Если из вершины east можно дойти до какой-то вершины, из которой мы пришли в вершину vertex,
			//Значит, vertex лежит в более крупной сильносвязной компоненте. Переназначаем lowlink
			t.lowlink[vertex] = std::min(t.lowlink[vertex], t.lowlink[east]);
		}
		else {
			//Если мы уже разбирали эту вершину, но она еще не ушла из стека
			//Значит, мы пришли в vertex из east. Тогда они в одной сильносвязной компоненте
			if (t.onStack[east]) {
				t.lowlink[vertex] = std::min(t.lowlink[vertex], t.index[east]);
			}
			//Иначе мы уже отнесли east к другой сильносвязной компоненте, которую уже проверили 
			//и vertex точно к ней не относится, оставляем все как есть
		}
	}



	//Если vertex оказывается корнем нашей полученной сильносвязной компоненты (никого выше по обходу не нашлось)
	//То очищаем стек и запоминаем эту сильносвязную компоненту (даем ей индекс и записываем в список sccIndexes)
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

//Функция алгоритма Тарьяна, которая заодно очищает связи между сильносвязными компонентами и вызывает более частную функцию
void ApplyTarjan(Tileset T) {
	if (T.size == 0) {
		return;
	}

	//Сортируем, чтобы все тайлы, у которых north цвет одинаковый, были рядом
	T.Sort();

	//Инициализируем структуру для алгоритма Тарьяна
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

	//Ищем, с какого тайла начинается какой цвет
	Color currentColor = -1;
	for (int i = 0; i < T.size; i++)
	{
		if (currentColor != T[i].north()) {
			currentColor = T[i].north();
			t.colorBeginningIndexes[currentColor] = i;
		}
	}

	//Выполняем алгоритм Тарьяна
	for (int i = 0; i < T.maxColor; i++)
	{
		if (t.index[i] == UNDEFINED_INDEX) {
			ApplyTarjan(t, T, i);
		}
	}

	//Проходимся по всем тайлам и удаляем те, у которых цвета из разных сильносвязных компонент
	//Для этого как бы пишем поверх ненужных тайлов нужные, а потом просто меняем размер
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