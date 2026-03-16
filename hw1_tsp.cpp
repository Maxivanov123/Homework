#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

struct City
{
    double x, y;
};

double dist(const City& a, const City& b)
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

double len(const std::vector<int>& t, const std::vector<std::vector<double>>& d)
{
    int n = t.size();
    double l = 0;
    for (int i = 0; i < n - 1; i++) l += d[t[i]][t[i + 1]];
    l += d[t[n - 1]][t[0]];
    return l;
}

void opt2(std::vector<int>& t, const std::vector<std::vector<double>>& d)
{
    int n = t.size();
    bool imp = true;

    while (imp)
    {
        imp = false;

        for (int i = 0; i < n - 1; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                int ni = (i + 1) % n;
                int nj = (j + 1) % n;

                double old = d[t[i]][t[ni]] + d[t[j]][t[nj]];
                double New = d[t[i]][t[j]] + d[t[ni]][t[nj]];

                if (New < old - 1e-10)
                {
                    std::reverse(t.begin() + i + 1, t.begin() + j + 1);
                    imp = true;
                }
            }
        }
    }
}

void opt3(std::vector<int>& t, const std::vector<std::vector<double>>& d)
{
    int n = t.size();
    bool imp = true;
    int iter = 0;

    while (imp && iter < 50)
    {
        imp = false;
        iter++;

        for (int i = 0; i < n - 2; i++)
        {
            for (int j = i + 1; j < n - 1; j++)
            {
                for (int k = j + 1; k < n; k++)
                {
                    int ni = (i + 1) % n;
                    int nj = (j + 1) % n;
                    int nk = (k + 1) % n;

                    double old = d[t[i]][t[ni]] + d[t[j]][t[nj]] + d[t[k]][t[nk]];

                    double v1 = d[t[i]][t[j]] + d[t[ni]][t[nj]] + d[t[k]][t[nk]];
                    double v2 = d[t[i]][t[j]] + d[t[ni]][t[nk]] + d[t[nj]][t[k]];
                    double v3 = d[t[i]][t[nj]] + d[t[j]][t[ni]] + d[t[nk]][t[k]];
                    double v4 = d[t[i]][t[nj]] + d[t[j]][t[nk]] + d[t[ni]][t[k]];

                    double best = std::min({ v1, v2, v3, v4 });

                    if (best < old - 1e-10)
                    {
                        if (best == v1)
                        {
                            std::reverse(t.begin() + i + 1, t.begin() + j + 1);
                            std::reverse(t.begin() + j + 1, t.begin() + k + 1);
                        }
                        else if (best == v2)
                        {
                            std::vector<int> nt;
                            for (int x = 0; x <= i; x++) nt.push_back(t[x]);
                            nt.push_back(t[j]);
                            for (int x = ni; x <= j; x++) nt.push_back(t[x]);
                            for (int x = nk; x < n; x++) nt.push_back(t[x]);
                            for (int x = nj; x <= k; x++) nt.push_back(t[x]);
                            if (nt.size() == n) t = nt;
                        }
                        else if (best == v3)
                        {
                            std::vector<int> nt;
                            for (int x = 0; x <= i; x++) nt.push_back(t[x]);
                            nt.push_back(t[nj]);
                            for (int x = j; x >= ni; x--) nt.push_back(t[x]);
                            for (int x = nk; x < n; x++) nt.push_back(t[x]);
                            for (int x = nj + 1; x <= k; x++) nt.push_back(t[x]);
                            if (nt.size() == n) t = nt;
                        }
                        else if (best == v4)
                        {
                            std::vector<int> nt;
                            for (int x = 0; x <= i; x++) nt.push_back(t[x]);
                            nt.push_back(t[nj]);
                            for (int x = nk; x < n; x++) nt.push_back(t[x]);
                            for (int x = ni; x <= j; x++) nt.push_back(t[x]);
                            nt.push_back(t[k]);
                            for (int x = nj + 1; x <= k; x++) nt.push_back(t[x]);
                            if (nt.size() == n) t = nt;
                        }

                        imp = true;
                        break;
                    }
                }
                if (imp) break;
            }
            if (imp) break;
        }
    }
}

int main()
{
    std::string file;
    std::cin >> file;

    std::ifstream f(file);
    if (!f.is_open()) return 1;

    int n;
    f >> n;

    std::vector<City> c(n);
    for (int i = 0; i < n; i++)
        f >> c[i].x >> c[i].y;

    f.close();

    std::vector<std::vector<double>> d(n, std::vector<double>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            d[i][j] = dist(c[i], c[j]);

    std::vector<int> t(n);
    std::vector<bool> v(n, false);

    t[0] = 0;
    v[0] = true;

    for (int k = 1; k < n; k++)
    {
        int b = -1;
        double bd = 1e20;

        for (int i = 0; i < n; i++)
        {
            if (!v[i])
            {
                double x = d[t[k - 1]][i];
                if (x < bd)
                {
                    bd = x;
                    b = i;
                }
            }
        }

        t[k] = b;
        v[b] = true;
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<int> bt = t;
    double bl = len(bt, d);

    for (int r = 0; r < 10; r++)
    {
        if (r > 0)
            std::shuffle(t.begin() + 1, t.end(), gen);

        opt2(t, d);
        opt3(t, d);

        double l = len(t, d);
        if (l < bl)
        {
            bl = l;
            bt = t;
        }
    }

    std::cout.precision(6);
    std::cout << std::fixed << bl << std::endl;

    return 0;
}
