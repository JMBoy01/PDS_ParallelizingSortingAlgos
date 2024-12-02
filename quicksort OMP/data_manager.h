#include <omp.h>
#include <vector>
#include <random>
#include <fstream>
#include <iostream>
#include <algorithm>

std::vector<int> generate_dataset(int dataset_size, int start_value = 0)
{
    std::vector<int> dataset(dataset_size);

    #pragma omp parallel for
    for (int i = 0; i < dataset_size; i++) {
        dataset[i] = i + start_value;
    }

    return dataset;
}

std::vector<int> generate_random_dataset(int dataset_size)
{
    std::vector<int> dataset(dataset_size);

    #pragma omp parallel for
    for (int i = 0; i < dataset_size; i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, dataset_size);

        dataset[i] = dis(gen);
    }

    return dataset;
}

// Fisher-Yates shuffle -> shuffles the given dataset
void shuffle_dataset(std::vector<int>& dataset)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::shuffle(dataset.begin(), dataset.end(), gen);
}

void save_dataset(const std::vector<int>& dataset, std::string filename)
{
    std::ofstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Fout bij het openen van het bestand!" << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(dataset.data()), dataset.size() * sizeof(int));
    file.close();
}

std::vector<int> load_dataset(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Fout bij het openen van het bestand!" << std::endl;
        return {};
    }

    // Verkrijg de grootte van het bestand om te bepalen hoeveel data we moeten inleiden
    file.seekg(0, std::ios::end);  // Ga naar het einde van het bestand
    int file_size = file.tellg();  // Verkrijg bestandsgrootte
    file.seekg(0, std::ios::beg);  // Ga terug naar het begin van het bestand

    // Bepaal hoeveel integers er in het bestand staan
    int num_elements = file_size / sizeof(int);

    std::vector<int> dataset(num_elements);  // Creëer een vector voor de data

    file.read(reinterpret_cast<char*>(dataset.data()), file_size);
    file.close();

    return dataset;
}
