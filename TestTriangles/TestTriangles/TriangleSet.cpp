#include "TriangleSet.h"

bool TriangleSet::isSimplePeriodic(Color rightMatchingColor)
{
	int i = rightMatchingColor * 2;

	for (i; i < rightColorsCount * 2; i += 2)
	{
		for (auto tr1 : triangles[i])
		{
			for (auto tr2 : triangles[i + 1])
			{
				if (tr1.left == tr2.left && tr1.top == tr2.top) {
					return true;
				}
			}
		}
	}
    return false;
}
