// CHOOSE A BITRATE

void sort_bitrate(int *list, int len)
{
    for (int i = 0; i < len; i++)
    {
        for (int j = i + 1; j < len; ++j)
        {
            if (list[i] > list[j])
            {
                int temp = list[j];
                list[j] = list[i];
                list[i] = temp;
            }
        }
    }
}

int choose_bitrate(double T_cur, int *list, int len)
{
    int i = 0;
    for (; i < len; ++i)
    {
        if (T_cur < 1.5 * list[i])
        {
            break;
        }
    }
    --i;
    i = i < 0 ? 0 : i;

    return list[i];
}
