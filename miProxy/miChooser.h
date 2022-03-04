// CHOOSE A BITRATE

void sort_bitrate(int *list, int len)
{
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < len - 1 - i; ++j)
        {
            if (list[j - 1] > list[j])
            {
                int temp = list[j];
                list[j] = list[j - 1];
                list[j - 1] = temp;
            }
        }
    }
}

int choose_bitrate(double T_cur, int *list, int len)
{
    sort_bitrate(list, len);

    int i = 0;
    for (; i < len; ++i)
    {
        if (T_cur < 1.5 * list[i])
        {
            break;
        }
    }
    i = i == len ? 0 : i;

    return list[i];
}
