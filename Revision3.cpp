#include <cstdlib>
#include <iostream>
#include <mutex>
#include <ctime>
#include <thread>

#define NUMBER_OF_PRODUCT_TYPES 40
#define NUMBER_OF_PRODUCERS 10
#define BATCH_SIZE 10
#define PRODUCTION_CYCLES 25000
#define NUMBER_OF_CUSTOMERS 5
#define CONSUMPTION_CYCLES 100000

typedef struct shared_memory{
	int products_available[NUMBER_OF_PRODUCT_TYPES];
	int products_produced[NUMBER_OF_PRODUCT_TYPES];
	int products_consumed[NUMBER_OF_PRODUCT_TYPES];
	std::mutex shared_memory_mutex;
} shared_memory;

void produceProducts(int n, shared_memory* memory);
void consumeProducts(int n, shared_memory* memory);
void printSummary(shared_memory* memory);


int main() {
	shared_memory memory;
	std::thread producer_threads[NUMBER_OF_PRODUCERS];
	std::thread customer_threads[NUMBER_OF_CUSTOMERS];

	for(int i = 0; i < NUMBER_OF_PRODUCT_TYPES; ++i) {
		memory.products_available[i] = 0;
		memory.products_produced[i] = 0;
		memory.products_consumed[i] = 0;
	}

	for(int i = 0; i < NUMBER_OF_PRODUCERS; ++i) {
		srand(std::hash<std::thread::id>{}(producer_threads[i].get_id()));
		producer_threads[i] = std::thread(produceProducts, i, &memory);
	}

	for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
		srand(std::hash<std::thread::id>{}(customer_threads[i].get_id()));
		customer_threads[i] = std::thread(consumeProducts, i, &memory);
	}

	for(int i = 0; i < NUMBER_OF_PRODUCERS; ++i) {
		producer_threads[i].join();
	}

	for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
		customer_threads[i].join();
	}
	printSummary(&memory);
	return 0;
}

void produceProducts(int n, shared_memory* memory) {
	for (int i = 0; i < PRODUCTION_CYCLES; ++i) {
		for (int j = 0; j < BATCH_SIZE; ++j) {
			int product = rand() % NUMBER_OF_PRODUCT_TYPES;
			std::lock_guard<std::mutex> lock(memory->shared_memory_mutex);
			memory->products_produced[product]++;
			memory->products_available[product]++;
		}
	}
}

void consumeProducts(int n, shared_memory* memory) {
	for (int i = 0; i < CONSUMPTION_CYCLES; ++i) {
		int product = rand() % NUMBER_OF_PRODUCT_TYPES;
		std::lock_guard<std::mutex> lock(memory->shared_memory_mutex);
		if (memory->products_available[product] > 0) {
			memory->products_available[product]--;
			memory->products_consumed[product]++;
		} else {
			i--;
		}
	}
}

void printSummary(shared_memory* memory) {
	int sum_produced = 0;
	int sum_consumed = 0;
	int sum_remaining = 0;

	std::cout << "INDEX\t\t|PRODUCED\t|CONSUMED\t|REMAINING\t|BALANCE\n";
	for (int i = 0; i < NUMBER_OF_PRODUCT_TYPES; ++i) {
		std::cout << "Product [" << i << "]\t|";
		std::cout << memory->products_produced[i] << "\t\t|";
		std::cout << memory->products_consumed[i] << "\t\t|";
		std::cout << memory->products_available[i] << "\t\t|";
		if (memory->products_produced[i] - memory->products_consumed[i] != memory->products_available[i]) {
			std::cout << "[incorrect]\n";
		} else {
			std::cout << "[correct]\n";
		}

		sum_produced += memory->products_produced[i];
		sum_consumed += memory->products_consumed[i];
		sum_remaining += memory->products_available[i];
	}

	std::cout << "\n----------In total--------\n";
	std::cout << "Produced: " << sum_produced << std::endl;
	std::cout << "Consumed: " << sum_consumed << std::endl;
	std::cout << "Remains: " << sum_remaining << std::endl;
	if ((sum_produced - sum_consumed) != sum_remaining) {
		std::cout << "Balance is incorrect!!!\n";
	} else {
		std::cout << "Overall balance is correct\n";
	}
}
