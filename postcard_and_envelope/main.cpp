#include <iostream>
#include <cmath>
#include <fstream>
#include <stdlib.h>

const double PI  = 3.141592653589793238463;

std::pair<float, float> rotate(std::pair<float, float> A, float angle)
{
    float r = std::sqrt(pow(A.first, 2)+pow(A.second, 2));
    A.first = r*cos(angle);
    A.second = r*sin(angle);

    return A;
}

bool change_coor(std::pair<int, int> postcard, std::pair<int, int> envelope)
{
    int hypotenuse_envelope = sqrt(pow(envelope.first, 2) + pow(envelope.second, 2));
    if (hypotenuse_envelope - postcard.second)
    {
        float h_postcard, w_postcard;
        std::pair<int, int> A(0, 0);
        std::pair<int, int> B(0, postcard.second);
        std::pair<int, int> C(postcard.first, 0);
        std::pair<int, int> D(postcard.first, postcard.second);
        for (float angle = 0; angle <= 2*PI;)
        {
            std::pair<float, float> AD(rotate(D, angle));
            std::pair<float, float> new_A(rotate(A, angle));
            AD.first -= new_A.first;
            AD.second -= new_A.second;
            std::pair<float, float> BC(rotate(C, angle));
            std::pair<float, float> new_B(rotate(B, angle));
            BC.first -= new_B.first;
            BC.second -= new_B.second;
            h_postcard = std::max(abs(BC.second), abs(AD.second));
            w_postcard = std::max(abs(BC.first), abs(AD.first));

            if ((h_postcard <= envelope.second)&&(w_postcard <= envelope.first))
                return true;
            angle += 0.001;

        }
    }
    else
        return false;
}


int main()
{
    std::fstream fin ("INPUT.txt");
    std::pair<int, int> postcard;
    std::pair<int, int> envelope;
    fin >> postcard.first >> postcard.second >> envelope.first >> envelope.second;
    fin.close();
    if (postcard.first > postcard.second)
        std::swap(postcard.first, postcard.second);
    if (envelope.first > envelope.second)
        std::swap(envelope.first, envelope.second);


    std::ofstream out("OUTPUT.TXT");
    if ((change_coor(postcard,envelope)))
        out << "Possible";
    else
        out << "Impossible";


    out.close();
    return 0;
}

