#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#define ALPHABET_SIZE 26
#define TOP_N 3

void read_distribution(const char *filename, double distribution[ALPHABET_SIZE]) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error at opening the file\n";
        return;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        file >> distribution[i];
        }

    file.close();
    }

void compute_histogram(const char *text, double histogram[ALPHABET_SIZE]) {
    int frequency[ALPHABET_SIZE] = {0};
    int text_len = strlen(text), nr_letters = 0;
    for (int i = 0; i < text_len; i++) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            frequency[text[i] - 'a']++;
            nr_letters++;
        } else if (text[i] >= 'A' && text[i] <= 'Z') {
            frequency[text[i] - 'A']++;
            nr_letters++;
        }
    }

    if (nr_letters == 0) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            histogram[i] = 0.0;
        }
        return;
    }

    double value = 100.0 / (double) nr_letters;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        histogram[i] = frequency[i] * value;
    }
}

double chi_squared_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double distance = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        distance += (hist1[i] - hist2[i]) * (hist1[i] - hist2[i]) / hist2[i];
    }
    return distance;
}

double cosine_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double sum1 = 0;
    double sum2 = 0;
    double sum3 = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        sum1 += hist1[i] * hist2[i];
        sum2 += hist1[i] * hist1[i];
        sum3 += hist2[i] * hist2[i];
    }
    double distance = 1.0 - (sum1 / (sqrt(sum2) * sqrt(sum3)));
    return distance;
}

double euclidian_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double distance = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        distance += (hist1[i] - hist2[i]) * (hist1[i] - hist2[i]);
    }
    return sqrt(distance);
}

void break_caesar_cipher(const char* text, int top_shifts[TOP_N], double top_distances[TOP_N], double (*distance_function)(const double[], const double[])) {
    int length = strlen(text);
    char text_copy[length + 1];
    double distribution[ALPHABET_SIZE], histogram[ALPHABET_SIZE];

    read_distribution("distribution.txt", distribution);

    for (int shift = 0; shift < ALPHABET_SIZE; shift++) {
        strcpy(text_copy, text);
        for (int i = 0; i < length; i++) {
            if (text_copy[i] >= 'a' && text_copy[i] <= 'z') {
                text_copy[i] = (text[i] - 'a' - shift + ALPHABET_SIZE) % ALPHABET_SIZE + 'a';
            } else if (text_copy[i] >= 'A' && text_copy[i] <= 'Z') {
                text_copy[i] = (text[i] - 'A' - shift + ALPHABET_SIZE) % ALPHABET_SIZE + 'A';
            }
        }

        compute_histogram(text_copy, histogram);
        double actual_distance = distance_function(histogram, distribution);

        if (actual_distance < top_distances[0]) {
            top_shifts[2] = top_shifts[1];
            top_distances[2] = top_distances[1];

            top_shifts[1] = top_shifts[0];
            top_distances[1] = top_distances[0];

            top_shifts[0] = shift;
            top_distances[0] = actual_distance;
        } else if (actual_distance < top_distances[1]) {
            top_shifts[2] = top_shifts[1];
            top_distances[2] = top_distances[1];

            top_shifts[1] = shift;
            top_distances[1] = actual_distance;
        } else if (actual_distance < top_distances[2]) {
            top_shifts[2] = shift;
            top_distances[2] = actual_distance;
        }
    }
    std::cout << top_distances[0] << " " << top_distances[1] << " " << top_distances[2] << " " << top_shifts[0] << " " << top_shifts[1] << " " << top_shifts[2] <<std::endl;
}


void encrypting_a_text_with_a_specific_shift(const char *initial_text, char encripted_text[], int shift) {
    int text_len = strlen(initial_text);
    for (int i = 0; i < text_len; i++) {
        if (initial_text[i] >= 'a' && initial_text[i] <= 'z') {
            encripted_text[i] = (initial_text[i] - 'a' + shift) % ALPHABET_SIZE + 'a';
        } else if (initial_text[i] >= 'A' && initial_text[i] <= 'Z') {
            encripted_text[i] = (initial_text[i] - 'A' + shift) % ALPHABET_SIZE + 'A';
        }else {
            encripted_text[i] = initial_text[i];
        }
    }
    encripted_text[text_len] = '\0';
}

void decrypting_a_text_with_a_specific_shift(const char *initial_text, char decripted_text[], int shift) {
    int text_len = strlen(initial_text);
    for (int i = 0; i < text_len; i++) {
        if (initial_text[i] <= 'z' && initial_text[i] >= 'a') {
            decripted_text[i] = (initial_text[i] - 'a' - shift + ALPHABET_SIZE) % ALPHABET_SIZE + 'a';
        } else if (initial_text[i] <= 'Z' && initial_text[i] >= 'A') {
            decripted_text[i] = (initial_text[i] - 'A' - shift + ALPHABET_SIZE) % ALPHABET_SIZE + 'A';
        } else {
            decripted_text[i] = initial_text[i];
        }
    }
    decripted_text[text_len] = '\0';
}

void computing_and_displaying_the_frequency_distribution_of_a_text(const char *text) {
double histogram[ALPHABET_SIZE];
    compute_histogram(text, histogram);
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        std::cout << char('A' + i) << ": " << histogram[i] << "%\n";
    }
}

void breaking_the_encrypted_text_using_frequency_analysis(const char* text,char decrypted_text[], double (*distance_function)(const double[], const double[])) {
    int top_shifts[TOP_N] = {-1, -1, -1};
    double top_distances[TOP_N] = {1e9, 1e9, 1e9};
    break_caesar_cipher(text, top_shifts, top_distances, distance_function);

    int shift = top_shifts[0];
    decrypting_a_text_with_a_specific_shift(text, decrypted_text, shift);

}

    int main() {
    int command;
        while (true) {
            std::cout << "Choose command:\n-1.Reading a text from the keyboard\n-2.Reading a text from a file\n-3.Encrypting a text with a specific shift\n-4.Decrypting a text with a known shift\n-5.Computing and displaying the frequency distribution of a text\n-6.Breaking the encrypted text using frequency analysis\n-0.Exit\n" << std::endl;
            std ::cin >> command;
            if (command == 0){
                break;
            } else if (command == 1) {
                std::cout << "Enter the text: ";
                char text[1001];
                std::cin >> text;
                std::cout << text << std::endl;
            } else if (command == 2) {
                break;
            } else if (command == 3) {
                int shift;
                char text[1001], encripted_text[1001];
                std::cout << "Enter the shift: ";
                std::cin >> shift;
                std::cin.ignore();
                std::cout << "Enter the text that you want to enript: " << std::endl;
                std::cin.getline(text, 1001);
                encrypting_a_text_with_a_specific_shift(text, encripted_text, shift);
                std::cout << "Ecripted text: " << encripted_text << std::endl;
            } else if (command == 4) {
                int shift;
                char text[1001], decrypted_text[1001];
                std::cin.ignore();
                std::cout << "Enter the encrypted text: ";
                std::cin.getline(text, 1001);

                std::cout << "Enter the shift value: ";
                std::cin >> shift;
                std::cin.ignore();

                decrypting_a_text_with_a_specific_shift(text, decrypted_text, shift);
                std::cout << "Decrypted text: " << decrypted_text << std::endl;

            } else if (command == 5) {
                char text[1001];
                std::cin.ignore();
                std::cout << "Enter the text: ";
                std::cin.getline(text, 1001);

                computing_and_displaying_the_frequency_distribution_of_a_text(text);
            } else if (command == 6) {
                char text[1001], decrypted_text[1001];
                std::cout << "Enter the enrypted text: ";
                std::cin.ignore();
                std::cin.getline(text, 1001);

                std::cout << "With what method do you want to use: \n 1. chi squared distance\n 2. cosine distance\n 3. euclidian distance\n";
                int method;
                std::cin >> method;
                std::cin.ignore();
                if (method == 1) {
                    breaking_the_encrypted_text_using_frequency_analysis(text,decrypted_text, chi_squared_distance);
                } else if (method == 2) {
                    breaking_the_encrypted_text_using_frequency_analysis(text, decrypted_text, cosine_distance);
                } else if (method == 3) {
                    breaking_the_encrypted_text_using_frequency_analysis(text,decrypted_text, euclidian_distance);
                } else {
                    std::cerr << "Invalid method!\n" << std::endl;
                }
                std::cout << "Decrypted text: " << decrypted_text << std::endl;
            } else {
                std::cerr << "Invalid command\n" << std::endl;
            }
        }
    }


/// - Which method consistently finds the correct shift?
///   Chi-Squared Distance consistently identified the correct shift for longer texts
///   Euclidean Distance and Cosine Distance had a higher error rate, especially for short texts, and also on long text their first, second and third shift are very usual the same

/// -Consider whether the structure of the text - such as letter distribution- affects decryption accuracy. If a text happens to contain unusual letter frequencies( for example , very few occurrences of 'E' or T' , which are common in English ) , does that influence the success rate ?
///  Shorter texts were more challenging to decrypt accurately
///  Longer texts provided better letter distribution, improving decryption accuracy

/// Texts with a natural distribution of letters in English (e.g., frequent 'E' and 'T') were decrypted more accurately.
/// Texts with unusual letter distributions or highly repetitive patterns resulted in more errors.

/// - Are there cases where the program selects the wrong shift?If so, why does this happen ?
///   Yes, there are cases when the program selects the wrong shift. This happen because of the length of the message. Shorter the message is, fewer chances of a correct decrypted answer are.
///   Another case when this could happen is when certain letters appear disproportionately in the taxt in order to the normal distribution


