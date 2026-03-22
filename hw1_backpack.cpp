#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>

struct Item
{
    int w, v;
};

int eval(const std::vector<int>& sol, const std::vector<Item>& items, int cap, int& totalWeight)
{
    int tw = 0, tv = 0;
    for (size_t i = 0; i < sol.size(); i++)
        if (sol[i]) 
        {
            tw += items[i].w; 
            tv += items[i].v; 
        }
    totalWeight = tw;
    return tw <= cap ? tv : 0;
}

std::vector<int> greedySolution(const std::vector<Item>& items, int cap)
{
    int n = items.size();
    std::vector<int> sol(n, 0);
    std::vector<int> idx(n);
    for (int i = 0; i < n; i++) 
        idx[i] = i;

    std::sort(idx.begin(), idx.end(), [&](int a, int b) 
        {
            return (double)items[a].v / items[a].w > (double)items[b].v / items[b].w;
        });

    int tw = 0;
    for (int i : idx)
    {
        if (tw + items[i].w <= cap)
        {
            sol[i] = 1;
            tw += items[i].w;
        }
    }

    return sol;
}

std::vector<int> hammingSearch(const std::vector<Item>& items, int cap, std::mt19937& gen)
{
    int n = items.size();
    std::uniform_int_distribution<> dis(0, 1);

    std::vector<int> best = greedySolution(items, cap);
    int bestWeight = 0;
    int bestVal = eval(best, items, cap, bestWeight);

    for (int run = 0; run < 100; run++)
    {
        std::vector<int> cur;
        if (run == 0)
        {
            cur = best;
        }
        else
        {
            cur = greedySolution(items, cap);
            for (int i = 0; i < 3; i++)
            {
                int idx = dis(gen) % n;
                cur[idx] = 1 - cur[idx];
            }
        }

        int curWeight = 0;
        int curVal = eval(cur, items, cap, curWeight);
        bool improved = true;

        while (improved)
        {
            improved = false;
            for (int i = 0; i < n; i++)
            {
                cur[i] = 1 - cur[i];
                int newWeight = 0;
                int newVal = eval(cur, items, cap, newWeight);
                if (newVal > curVal)
                {
                    curVal = newVal;
                    curWeight = newWeight;
                    improved = true;
                }
                else
                {
                    cur[i] = 1 - cur[i];
                }
            }
        }

        if (curVal > bestVal)
        {
            bestVal = curVal;
            bestWeight = curWeight;
            best = cur;
        }
    }

    return best;
}

int main()
{
    std::string filename;
    std::cin >> filename;

    std::ifstream f(filename.c_str());
    if (!f.is_open())
    {
        std::cerr << "Error: Cannot open file" << std::endl;
        return 1;
    }

    int n, cap;
    f >> n >> cap;

    std::vector<Item> items(n);
    for (int i = 0; i < n; i++)
        f >> items[i].w >> items[i].v;
    f.close();

    bool canTakeMultiple = false;
    for (int i = 0; i < n && !canTakeMultiple; i++)
        for (int j = i + 1; j < n; j++)
            if (items[i].w + items[j].w <= cap)
                canTakeMultiple = true;

    int result = 0;
    int totalWeight = 0;
    std::vector<int> selected;

    if (canTakeMultiple)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::vector<int> bestSol = hammingSearch(items, cap, gen);
        result = eval(bestSol, items, cap, totalWeight);
        selected = bestSol;
    }
    else
    {
        int bestIdx = -1;
        int bestVal = 0;
        for (int i = 0; i < n; i++)
        {
            if (items[i].w <= cap && items[i].v > bestVal)
            {
                bestVal = items[i].v;
                bestIdx = i;
            }
        }
        result = bestVal;
        if (bestIdx != -1)
        {
            selected.resize(n, 0);
            selected[bestIdx] = 1;
            totalWeight = items[bestIdx].w;
        }
    }

    std::cout << "Best value: " << result << std::endl;
    std::cout << "Total weight: " << totalWeight << " / " << cap << std::endl;
    std::cout << "Selected items: ";

    bool first = true;
    for (int i = 0; i < n; i++)
    {
        if (selected[i] == 1)
        {
            if (!first) std::cout << ", ";
            std::cout << i + 1 << " (w=" << items[i].w << ", v=" << items[i].v << ")";
            first = false;
        }
    }
    std::cout << std::endl;

    return 0;
}
