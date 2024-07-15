#include <string>
#include <iostream>
#include <vector>
#include <random>

std::string randomColor() {
	std::vector<std::string> colors = { "White", "Black", "Green", "Purple", "Red" };
	int i = rand() % 5;
	return colors[i];
}

std::string randomMaterial() {
	std::vector<std::string> colors = { "Carbon", "Metall", "Plastick", "Tree", "Gold" };
	int i = rand() % 5;
	return colors[i];
}

class InterfaceCar {
public:
	virtual void PrintСharacteristic() {}
	virtual ~InterfaceCar() = default;
};

class Mers : public InterfaceCar {
	std::string color;
	std::string material;
public:
	Mers(std::string Mcolor, std::string Mmaterial) {
		this->color = Mcolor;
		this->material = Mmaterial;
	}
	void PrintСharacteristic(){
		std::cout << "Marka: Mers | " << "Color: " << this->color << " | Material: " << this->material<<std::endl;
	
	}
};

class Bexa : public InterfaceCar {
	std::string color;
	std::string material;
public:
	Bexa(std::string Bcolor, std::string Bmaterial) {
		this->color = Bcolor;
		this->material = Bmaterial;
	}
	void PrintСharacteristic() {
		std::cout << "Marka: Bexa | " << "Color: " << this->color << " | Material: " << this->material << std::endl;

	}
};

class Audi : public InterfaceCar {
	std::string color;
	std::string material;
public:
	Audi(std::string Acolor, std::string Amaterial) {
		this->color = Acolor;
		this->material = Amaterial;
	}
	void PrintСharacteristic() {
		std::cout << "Marka: Audi | " << "Color: " << this->color << " | Material: " << this->material << std::endl;

	}
};

class FabricCar {
public:
	virtual std::shared_ptr<InterfaceCar> createCar() const = 0;
	virtual ~FabricCar() = default;
};

class MersFabric : public FabricCar {
public:
	std::shared_ptr<InterfaceCar> createCar() const override {
		return std::make_shared<Mers>(randomColor(), randomMaterial());
	}
};

class BexaFabric : public FabricCar {
public:
	std::shared_ptr<InterfaceCar> createCar() const override {
		return std::make_shared<Bexa>(randomColor(), randomMaterial());
	}
};

class AudiFabric : public FabricCar {
public:
	std::shared_ptr<InterfaceCar> createCar() const override {
		return std::make_shared<Audi>(randomColor(), randomMaterial());
	}
};


int main() {
	srand(time(NULL));

	int N, nA, nB,nC;
	std::cout << "Quality Car : ";
	std::cin >> N;
	std::cout <<std::endl<<"Mers : ";
	std::cin >> nA;
	std::cout << std::endl << "Bexa : ";
	std::cin >> nB;
	std::cout << std::endl << "Audi : ";
	std::cin >> nC;

	if ((nA + nB + nC) > N) {
		std::cout << "Incorrect input" << std::endl;
		return 1;
	}

	std::vector <std::shared_ptr<InterfaceCar>> Cars;
	MersFabric MersF;
	BexaFabric BexaF;
	AudiFabric AudiF;
	for (int i = 0; i < nA; i += 1) {
		Cars.push_back(MersF.createCar());
	}
	for (int i = 0; i < nB; i += 1) {
		Cars.push_back(BexaF.createCar());
	}
	for (int i = 0; i < nC; i += 1) {
		Cars.push_back(AudiF.createCar());
	}

	std::shuffle(Cars.begin(), Cars.end(), std::mt19937(std::random_device()()));
	for (int i = 0;i<Cars.size();i+=1) {
		Cars[i]->PrintСharacteristic();
	}

}
