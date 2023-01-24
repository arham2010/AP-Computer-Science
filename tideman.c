#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;


// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void merge_sort(int left_side, int right_side);
void lock_pairs(void);
bool makes_cycle(int current_vertex_winner, int original_vertex_winner, int original_vertex_loser, bool start);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }


    add_pairs();

    sort_pairs();

    lock_pairs();

    print_winner();

    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int iter = 0; iter < candidate_count; iter++)
    {
        // Check if any candidate matches with name
        if (strcmp(candidates[iter], name) == 0)
        {
            // Update votes
            ranks[rank] = iter;
            return true;
        }

    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{

    int checker_candidate = 0;
    int current_candidate = 0;

    // Iter's through candidates
    for (int current_iteration = 0; current_iteration < candidate_count; current_iteration++)
    {
        for (int iteration2 = 0; iteration2 < candidate_count; iteration2++)
        {

            if (current_iteration == iteration2)
            {
                continue;
            }

            // Find index of candidate in ranks
            for (int search_iter = 0; search_iter < candidate_count; search_iter++)
            {
                if (ranks[search_iter] == iteration2)   // Person is in the column of preferences
                {
                    checker_candidate = search_iter + 1 ;
                }
                else if (ranks[search_iter] == current_iteration)
                    // Person is in the row of preferences, ie, the person that we want know has votes over ppl
                {
                    current_candidate = search_iter + 1;
                }
            }

            // If people like one candidate over another
            if (current_candidate < checker_candidate)
            {
                preferences[current_iteration][iteration2]++;
            }

        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int won;
    int lost;
    for (int current_iter = 0; current_iter < candidate_count; current_iter++)
    {
        for (int iter2 = current_iter + 1; iter2 < candidate_count; iter2++)
        {
            if (preferences[current_iter][iter2] > preferences[iter2][current_iter])
            {
                pairs[pair_count].winner = current_iter;
                pairs[pair_count].loser = iter2;
                pair_count++;
            }

            else if (preferences[current_iter][iter2] < preferences[iter2][current_iter])
            {
                pairs[pair_count].winner = iter2;
                pairs[pair_count].loser = current_iter;
                pair_count++;
            }

        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    merge_sort(0, pair_count - 1);
}

void merge_sort(int left_side, int right_side)
{
    // if nothing to sort, doesn't enter the loop
    if (left_side != right_side)
    {
        // 1) Declaring Values
        int middle = (left_side + right_side) / 2;

        // 2) Recursion
        merge_sort(left_side, middle);  // Call left side.
        merge_sort(middle + 1, right_side); // Call right side. Middle + 1 becuase middle is already used in left side.

        // 3) Merging
        pair temporary_array[pair_count];

        for (int iter = 0; iter < pair_count; iter++)
        {
            temporary_array[iter] = pairs[iter];
        }

        int left_counter = left_side; // Start of left side list
        int right_counter = middle + 1; // Start of right side list
        int temp_counter = left_side; // Start of temp list

        // Add values to temp

        while (left_counter <= middle && right_counter <= right_side)
        {
            int comparison_left = preferences[pairs[left_counter].winner][pairs[left_counter].loser] -
                                  preferences[pairs[left_counter].loser][pairs[left_counter].winner];
            int comparison_right = preferences[pairs[right_counter].winner][pairs[right_counter].loser] -
                                   preferences[pairs[right_counter].loser][pairs[right_counter].winner];

            // Check to see if variable in right or left list is bigger, and add them to temp
            if (comparison_left <= comparison_right)
            {
                // Number on left is smaller
                temporary_array[temp_counter] = pairs[right_counter];
                right_counter++;
            }
            else
            {
                // Number on right is smaller
                temporary_array[temp_counter] = pairs[left_counter];
                left_counter++;
            }
            temp_counter++;
        }

        // If left side runs out

        while (right_counter <= right_side)
        {
            temporary_array[temp_counter] = pairs[right_counter];
            right_counter++;
            temp_counter++;
        }

        // If right side runs out

        while (left_counter <= middle)
        {
            temporary_array[temp_counter] = pairs[left_counter];
            left_counter++;
            temp_counter++;
        }

        for (int iter = 0; iter <= right_side; iter++)
        {
            pairs[iter] = temporary_array[iter];
        }

        return;
    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int iter = 0; iter < pair_count; iter++)
    {
        // First pair will not cause cycle
        if (iter == 0)
        {
            locked[pairs[iter].winner][pairs[iter].loser] = true;
        }
        else
        {
            locked[pairs[iter].winner][pairs[iter].loser] = true;

            if (makes_cycle(pairs[iter].winner, pairs[iter].winner, pairs[iter].loser, true))
            {
                locked[pairs[iter].winner][pairs[iter].loser] = false;
            }
        }
    }
    return;
}


bool makes_cycle(int current_vertex_winner, int original_vertex_winner, int original_vertex_loser, bool start)
{
    bool cycle = false;
    if (current_vertex_winner == original_vertex_winner && !start)
    {
        cycle = true;
        return cycle;
    }
    for (int iter = 0; iter < candidate_count; iter++)
    {
        if (locked[current_vertex_winner][iter])
        {
            if (makes_cycle(iter, original_vertex_winner, original_vertex_loser, false))
            {
                return true;
            }
        }
    }
    return cycle;
}


// Print the winner of the election
void print_winner(void)
{
    int is_source[candidate_count];

    for (int i = 0; i < candidate_count; i++)
    {
        is_source[i] = true;
    }

    for (int iter = 0; iter < candidate_count; iter++)
    {
        for (int iter2 = 0; iter2 < candidate_count; iter2++)
        {
            if (locked[iter2][iter])
            {
                is_source[iter] = false;
            }
        }
    }

    for (int j = 0; j < candidate_count; j++)
    {
        if (is_source[j])
        {
            printf("%s\n", candidates[j]);
        }
    }
    return;
}
