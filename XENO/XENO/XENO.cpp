
#include "stdafx.h"
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <iostream>
#include "XENO.h"
using namespace std;

const int HPr = 20;		//Коэффициент лечения
const int SlDw = 20;	//Замедление заражения

smp symptom[12];		//12 симптомов
twn town[5];			//5 поселений
vector<vir> virus;		//3 стартовых вируса и последующие вирусы. Используется для заражения и для изучения.
vector<med> drug(3);	//3 стартовых лекарства: Аспасин, Ультрамарин, Неподыхан. Также хранит синтезированные лекарства
int day = 1, qrt = 0;	//Счетчик дней и счетчик установленных на карантин поселений (на карантин можно закрыть не более двух поселений)
bool win = false;		//Для функции game_over()

						//Начало ининциализации
void init_smp()
{
	string name;
	int s = 0;
	for (int i = 0; i < 12; i++)
	{
		switch (i)
		{
			//Легкие
		case 0: name = "Мигрень"; break;
		case 1: name = "Повышенная температура"; break;
		case 2: name = "Нервный тик"; break;
			//Заразные
		case 3: name = "Кашель"; break;
		case 4: name = "Сыпь"; break;
		case 5: name = "Воспаление слизистой"; break;
			//Тяжелые
		case 6: name = "Потеря зрения"; break;
		case 7: name = "Кишечный узел"; break;
		case 8: name = "Галлюцинации"; break;
			//Смертельные
		case 9: name = "Выстрел третьего глаза"; break;
		case 10: name = "Детонация"; break;
		case 11: name = "Растекание"; break;
		}

		if (i >= 0) symptom[i].init(name, 1, 5, 0);
		if (i > 2) symptom[i].init(name, 2, 25, 0);
		if (i > 5) symptom[i].init(name, 3, 50, 1);
		if (i > 8) symptom[i].init(name, 4, 75, 20);

		s++;
		if (s >= 2) s = 0;
	}
	cout << "Симптомы инициализированы.\n";
}

void init_vir()
{
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 3; i++)
	{
		virus.push_back(vir());
		for (int j = 0; j < 4; j++)
		{
			virus[i].symptom[j] = symptom[(rand() % 3) + j * 3];
		}
	}

	cout << "Штаммы вирусов инициализированы.\n";
}

bool check_med(int lvl)
{
	//Условие проверки: 
	//				1) ПЭ и ОЭ не должны быть одинаковы;
	//				2) У лекарств не должно быть одинаковой пары ПЭ и ОЭ
	for (int i = 0; i < 3; i++)
	{
		if (drug[i].GE[lvl].name == drug[i].BE[lvl].name) return true;
	}
	return false;
}
void init_med()
{
	//Условие: должны быть использованы ВСЕ симптомы, но ни одно лекарство не должно идеально подходить к вирусу.
	//Положительные и отрицательные эффекты не должны повторяться. Однако, это допускается при синтезе лекарства.
	vector<smp> v_smp(3);

	srand(static_cast<unsigned int>(time(0)));
	//Подбор симптомов по уровням
	for (int lvl = 0; lvl < 4; lvl++)
	{
		//Подбор симптомов из уровня
		for (int i = 0; i < 3; i++)
		{
			v_smp[i] = symptom[i + 3 * lvl];
		}
		//Загрузка симптомов определенного уровня в стартовые лекарства
		//Цикл нужен для проверки повторения симптомов.
		//Если проверка не будет пройдена, симптомы перетасуются снова.
		do
		{
			//Загрузка положительных эффектов
			random_shuffle(v_smp.begin(), v_smp.end());
			for (int i = 0; i < 3; i++)
			{
				drug[i].GE[lvl] = v_smp[i];
				drug[i].synth_day = 0;
			}
			//Загрузка побочных эффектов
			random_shuffle(v_smp.begin(), v_smp.end());
			for (int i = 0; i < 3; i++)
			{
				drug[i].BE[lvl] = v_smp[i];
			}
		} while (check_med(lvl));
	}
	drug[0].name = "Аспасин";
	drug[1].name = "Ультрамарин";
	drug[2].name = "Неподыхан";

	v_smp.~vector();
	cout << "Лекарства инициализированы.\n";
}

void init_twn()
{
	string name;
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 5; i++)
	{
		switch (i)
		{
		case 0: name = "Дун-Дук"; break;
		case 1: name = "Гу-Га"; break;
		case 2: name = "Дефи-Дерде"; break;
		case 3: name = "Маки-Ша"; break;
		case 4: name = "Жо-Жо"; break;
		}

		town[i].init(name, virus[rand() % 3]);
	}
	cout << "Поселения инициализированы.\n";
}

bool initialisation()
{
	cout << "Инициализация...\n";
	virus.reserve(15);
	drug.reserve(20);

	//Инициализация симптомов. По три на каждый уровень.
	init_smp();

	//Инициализация стартовых вирусов
	init_vir();

	//Инициализация лекарств.
	init_med();

	//Инициализация городов
	init_twn();

	//Прочее
	//storage->reserve(15);

	return true;
}
//Конец инициализации


//Вспомогательные функции
bool is_digit(string str)
{
	bool digit = true;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		if (str[i] < '0' || str[i] > '9')
		{
			digit = false;
			break;
		}
	}
	return digit;
}

int input()
{
	string str;
	unsigned int i, j;

	do
	{
		cout << endl << ">";
		cin >> str;
		transform(str.begin(), str.end(), str.begin(), tolower);

		if (str == "help")
		{
			cout << "\nКоманды:\nИнформация о лекарствах: 'drugs' или 'medicine'\n";
			cout << "Информация о вирусах: 'virus'\n";
			cout << "Информация о поселениях: 'towns'\n";
			cout << "Выход из игры: 'exit'\n";
			continue;
		}
		if (str == "drugs" || str == "medicine")
		{
			for (i = 0; i < drug.size(); i++)
			{
				cout << "\n-----" << drug[i].name << "-----" << endl;
				if (drug[i].synth_day) cout << "\n(Синтезируется. Осталось " << drug[i].synth_day << " дней)\n";
				cout << "Помогает против:" << endl;
				for (j = 0; j < 4; j++)
				{
					cout << ((drug[i].GE[j].researched) ? "-" + drug[i].GE[j].name : "<Неизвестно>") << endl;
				}
				cout << "Побочные эффекты:" << endl;
				for (j = 0; j < 4; j++)
				{
					cout << ((drug[i].BE[j].researched) ? "-" + drug[i].BE[j].name : "<Неизвестно>") << endl;
				}
			}
			continue;
		}
		if (str == "virus")
		{
			for (i = 0; i < virus.size(); i++)
			{
				if (virus[i].probed)
				{
					cout << "\n-----" << virus[i].name << "-----" << endl;
					cout << "Симптомы:" << endl;
					for (j = 0; j < 4; j++)
					{
						cout << ((virus[i].symptom[j].researched) ? "-" + virus[i].symptom[j].name : "<Неизвестно>") << endl;
					}
				}
			}
			continue;
		}
		if (str == "towns")
		{
			for (int i = 0; i < 5; i++)
			{
				if (town[i].t_dead)
				{
					cout << town[i].name << endl;
				}
				else if (!town[i].p_infected) { cout << "Население " + town[i].name + " (" << town[i].p_amount << ") вылечено от вируса " + town[i].virus->name << endl; }
				else
				{
					cout << town[i].name << " (" << town[i].p_amount << ")" << ": зараженных вирусом " +
						((town[i].probe_taken) ? town[i].virus->name : "<Неизвестный вирус>") + " " << town[i].p_infected << ", что составляет "
						<< (int)((double)town[i].p_infected*100. / town[i].p_amount) << "% от численности поселения\n";
				}
			}
			continue;
		}
		if (str == "exit")
		{
			exit(0);
		}
		if (is_digit(str))
		{
			return atoi(str.c_str());
		}

		cout << "Неизвестная команда\n";

	} while (true);

}
//Конец вспомогательных функций


//Игровые функции
void use_drug(int c)
{
	unsigned int d, i;
	cout << "Выберите одно из лекарств:\n";
	for (i = 0; i < drug.size(); i++)
	{
		if (drug[i].synth_day)
		{
			//i--;
			break;
		}
		cout << i + 1 << ". " + drug[i].name << endl;
	}
	do
	{
		d = input();
		if (d < 1 || d > i) cout << "\nВыберите одно из существующих лекарств.\n";
	} while (d < 1 || d > i);
	d--;

	//Проверяется совпадение симптомов от высшего вниз. Каждое совпадение +10% к лечению и -20% к скорости заражения.
	int heal_precent = 0, slowdown = 0;
	//for (int i = town[c].active_symptom.lvl - 1; i >= 0; i--)	//Лечение по активным симптомам
	for (int i = 3; i >= 0; i--)								//Лечение по всем симтомам
	{
		if (town[c].virus->symptom[i].name == drug[d].GE[i].name)
		{
			heal_precent += HPr;
			slowdown += SlDw;
		}
	}
	//Проверка на побочные эффекты. Здесь уже проверяются ВСЕ симптомы вируса. Каждое совпадение -10% к лечению. Скорость заражения сбрасывается.
	for (int i = 3; i >= 0; i--)
	{
		if (town[c].virus->symptom[i].name == drug[d].BE[i].name)
		{
			heal_precent -= 10;
			if (!slowdown) slowdown -= SlDw;
		}
	}

	town[c].heal(heal_precent, slowdown, drug[d].name);
}

void take_probe(int c)
{
	string vir_name;
	//Проверка: встречался ли уже вирус. Если да, то поселению С параметр probe_taken будет равен true
	for (unsigned int i = 0; i < virus.size(); i++)
	{
		if (&virus[i] == town[c].virus)
		{
			if (virus[i].probed)
			{
				cout << "Этот штамм вам уже встречался ранее. Это " + town[c].virus->name << endl;
				town[c].probe_taken = true;
			}
			else
			{
				cout << "Вы обнаружили новый штамм вируса! Назовите его: ";
				cin >> vir_name;
				town[c].virus->name = vir_name;
				town[c].probe_taken = true;
				virus[i].probed = true;
				virus[i].symptom[town[c].active_symptom.lvl - 1].researched = true;
			}
			break;
		}
	}
}

void set_quarantine(int c)
{
	char q;
	if (town[c].quarantine)
	{
		cout << "Вы уверены в своем решении? День все равно будет потрачен, надо было тише говорить о своих планах! (+/-)\n";
		cin >> q;
		if (q == '+')
		{
			town[c].quarantine = false;
			cout << "В поселении " << town[c].name << " снят карантин.\n"; qrt--;
		}
		else cout << "Вы ни с чем вернулись на базу, впустую потратив день и подняв панику в поселении.\n";
	}
	else
	{
		if (qrt >= 1)
		{
			cout << "Вы не можете закрыть более одного поселения на карантин!\n";
		}
		else
		{
			cout << "Вы уверены в своем решении? День все равно будет потрачен, надо было тише говорить о своих планах! (+/-)\n";
			cin >> q;
			if (q == '+')
			{
				town[c].quarantine = true;
				cout << "В поселении " << town[c].name << " установлен карантин.\n"; qrt++;
			}
			else cout << "Вы ни с чем вернулись на базу, впустую потратив день и подняв панику в поселении.\n";
		}
	}
}

bool check_vir_research(vir& r_virus)
{
	if (r_virus.researched)
	{
		cout << r_virus.name + " уже исследован. Выберите другой штамм.\n";
		return true;
	}
	if (r_virus.name == "<Неизвестный вирус>")
	{
		cout << "Вы еще не получили образец этого вируса, чтобы исследовать его!\n";
		return true;
	}
	return false;
}
void vir_research()
{
	unsigned int i, vir_c, med_c;
	do
	{
		cout << "Выберите один из имеющихся штаммов вируса:\n";
		for (i = 0; i < virus.size(); i++)
		{
			cout << i + 1 << ". " + virus[i].name << endl;
		}
		cout << "0. Отмена (День будет утерян)\n";
		do
		{
			vir_c = input();
			cout << endl;
			if (vir_c == 0) return;	//Отмена
			if (vir_c < 1 || vir_c > i) cout << "Выберите существующий штамм.\n";
		} while (vir_c < 1 || vir_c > i);
		vir_c--; //Для работы с массивом
	} while (!&virus[vir_c]);

	cout << "Выберите препарат, который используете на вирус:\n";
	for (i = 0; i < drug.size(); i++)
	{
		cout << i + 1 << ". " + drug[i].name << endl;
	}
	do
	{
		med_c = input();
		cout << endl;
		if (med_c < 1 || med_c > i) cout << "Выберите существующий препарат.\n";
	} while (med_c < 1 || med_c > i);
	med_c--; //Для работы с массивом

			 //Поиск совпадений симптомов вируса с ОЭ лекарства
	for (i = 0; i < 4; i++)
	{
		if (virus[vir_c].symptom[i].name == drug[med_c].BE[i].name && !virus[vir_c].symptom[i].researched)
		{
			virus[vir_c].symptom[i].researched = true;
			cout << "Вы определили симптом " << i + 1 << " уровня вируса " + virus[vir_c].name + ": " + virus[vir_c].symptom[i].name << endl;
			virus[vir_c].research_check(); //Проверка на исследованность
			drug[med_c].BE[i].researched = true; //ОЭ, раскрывший потенциал вируса, становится известным
			return;
		}
	}
	cout << "Препарат " + drug[med_c].name + " не смог раскрыть потенциал вируса " + virus[vir_c].name << endl;
}

bool check_med_research(int c)
{
	for (int i = 0; i < 4; i++)
	{
		if (!drug[c].BE[i].researched || !drug[c].BE[i].researched)
		{
			cout << "\nВы выбрали " + drug[c].name + " для исследования.\n";
			return false;
		}
	}
	cout << "Данный препарат уже был изучен. Укажите другой препарат.";
	return true;
}
void med_research()
{
	unsigned int i, c, v, r;
	cout << "\nСлучайным образом будет открыт один из эффектов лекарства.\n";
	do
	{
		cout << "Выберите препарат, который хотите исследовать:\n";
		for (i = 0; i < drug.size(); i++)
		{
			if (drug[i].synth_day)
			{
				//i--;
				break;
			}
			cout << i + 1 << ". " + drug[i].name << endl;
		}
		cout << "0. Отмена (День будет утерян)\n";

		do
		{
			c = input();
			cout << endl;
			if (c == 0) return;	//Отмена
			if (c < 1 || c > i) cout << "Выберите существующий препарат.\n";
		} while (c < 1 || c > i);
		c--; //Для работы с массивом
	} while (check_med_research(c));

	//Исследование эффектов
	srand(static_cast<unsigned int>(time(0)));
	do
	{
		v = rand() % 2;
		if (v == 0)
		{
			r = rand() % 4;
			if (drug[c].GE[r].researched) continue;
			cout << "Вы открыли новый эффект лекарства! " + drug[c].name + " помогает против " + drug[c].GE[r].name + "\n\n";
			drug[c].GE[r].researched = true;
			break;
		}
		else
		{
			r = rand() % 4;
			if (drug[c].BE[r].researched) continue;
			cout << "Вы открыли новый эффект лекарства! " + drug[c].name + " имеет побочный эффект: " + drug[c].BE[r].name + "\n\n";
			drug[c].BE[r].researched = true;
			break;
		}
		cout << drug[c].name + " исследован.";
		break;
	} while (true);
}

void research()
{
	int c;
	cout << "\n1.Исследование вирусов.\n2.Исследование лекарств.\n";
	do
	{
		c = input();
		if (c < 1 || c > 2) cout << "Выберите одно из предложенных исследований.\n";
	} while (c < 1 || c > 2);

	switch (c)
	{
	case 1:
		cout << "\nНужно применить на штамм один из препаратов чтобы выявить его симптомы.\n";
		vir_research();
		break;
	case 2:
		med_research();
		break;
	}
}

void synth()
{
	cout << "\nДля синтеза необходимо выбрать два перпарата и один из исследованных штаммов вируса.\n";
	unsigned int n = 0, i, vir_c, drug_c1, drug_c2;

	//Создание вектора ссылок для меню выбора ТОЛЬКО исследованных вирусов
	vector<vir*> synth_vir;
	//Заполнение вектора исследованными вирусами
	for (i = 0; i < virus.size(); i++)
	{
		if (virus[i].researched)
		{
			synth_vir.push_back(&virus[i]);
			n++;
		}
	}
	if (n == 0)
	{
		cout << "Похоже, у вас нет исследованных штаммов для синтеза лекарства.\nВы потратили день впустую, собирая обратно полевую лабораторию.\n";
		synth_vir.~vector();
		return;
	}

	//Выбор штамма вируса для синтеза
	cout << "Выберите штамм вируса:\n";
	for (i = 0; i < synth_vir.size(); i++)
	{
		cout << i + 1 << ". " + synth_vir[i]->name << endl;
	}
	do
	{
		vir_c = input();
		cout << endl;
		if (vir_c < 1 || vir_c > i) cout << "Выберите существующий штамм.\n";
	} while (vir_c < 1 || vir_c > i);
	vir_c--;

	//Выбор лекарств
	cout << "Выберите препараты, который будут применены для синтеза лекарства:\n";
	do
	{
		for (i = 0; i < drug.size(); i++)
		{
			cout << i + 1 << ". " + drug[i].name << endl;
		}

		do
		{
			drug_c1 = input();
			cout << endl;
			if (drug_c1 < 1 || drug_c1 > i) cout << "Выберите существующий препарат.\n";
		} while (drug_c1 < 1 || drug_c1 > i);
		drug_c1--; //Для работы с массивом

		do
		{
			drug_c2 = input();
			cout << endl;
			if (drug_c2 < 1 || drug_c2 > i) cout << "Выберите существующий препарат.\n";
		} while (drug_c2 < 1 || drug_c2 > i);
		drug_c2--; //Для работы с массивом

		if (drug_c1 == drug_c2) cout << "Необходимо выбрать два разных препарата!\n";
	} while (drug_c1 == drug_c2);

	cout << "Синтез лекарства начался. Через 7 дней препарат будет готов к использованию.\n";
	drug.push_back(med()); int new_drug = drug.size() - 1;
	cout << "Назовите ваш новый препарат: ";
	cin >> drug[new_drug].name;

	//Подбор ПЭ нового лекарства по совпадению с ОЭ (приоритет первому лекарству)
	for (i = 0; i < 4; i++)
	{
		if (synth_vir[vir_c]->symptom[i].name == drug[drug_c1].BE[i].name)
		{
			drug[new_drug].GE[i] = drug[drug_c1].BE[i];
			drug[new_drug].GE[i].researched = true;
		}
		else if (synth_vir[vir_c]->symptom[i].name == drug[drug_c2].BE[i].name)
		{
			drug[new_drug].GE[i] = drug[drug_c2].BE[i];
			drug[new_drug].GE[i].researched = true;
		}
		else   //Если совпадений нет, подтянется случайный ОЭ из массива symptom[]
		{
			drug[new_drug].GE[i] = symptom[(rand() % 3) + i * 3];
		}
	}

	//Побор ОЭ нового лекарства путем выбора ПЭ одного из двух лекарств-родителей
	srand(static_cast<unsigned int> (time(0)));
	for (i = 0; i < 4; i++)
	{
		if (rand() % 2)
		{
			drug[new_drug].BE[i] = drug[drug_c1].GE[i];
		}
		else
		{
			drug[new_drug].BE[i] = drug[drug_c2].GE[i];
		}
	}

	synth_vir.~vector();
	return;
}

void synth_day()
{
	vector<med>::iterator i_drug;
	for (i_drug = drug.begin(); i_drug != drug.end(); i_drug++)
	{
		if (i_drug->synth_day == 1) cout << "(+) Лекарство '" + i_drug->name + "' синтезировано и готово к применению!\n";
		if (i_drug->synth_day) i_drug->synth_day--;
	}
}

void mutate()
{
	int t1, t2;

	//Выбор двух поселений, между которыми произойдет слияние вирусов.
	srand(static_cast<unsigned int>(time(0)));
	int count = 0; //Костыль!!! Для выхода из бесконечного цикла. Если подоходящих поселений нет, может образоваться бесконечный цикл
	do
	{
		t1 = rand() % 5;
		t2 = rand() % 5;
		++count;
		if (count > 5)
		{
			cout << "Вирус не смог мигрировать в другое поселение\n";
			break;
		}
	} while (town[t1].quarantine || town[t2].quarantine || t1 == t2 || !town[t1].p_infected || !town[t2].p_infected); //Вылеченные поселения пока не трогаются
	cout << "(!)Вирус из поселения " + town[t1].name + " попал в поселение " + town[t2].name + " и образовал новый вид вируса\n";

	//Создание нового вируса в векторе
	virus.push_back(vir());
	unsigned int c = virus.size() - 1;

	//Наполнение вируса симптомами от вирусов-родителей (по два от каждого)
	srand(static_cast<unsigned int>(time(0)));
	vector<int> rand_smp = { 0, 1, 2, 3 };
	random_shuffle(rand_smp.begin(), rand_smp.end());
	for (int i = 0; i < 4; i++)
	{
		if (i < 2)
		{
			virus[c].symptom[rand_smp[i]] = town[t1].virus->symptom[rand_smp[i]];
		}
		else
		{
			virus[c].symptom[rand_smp[i]] = town[t2].virus->symptom[rand_smp[i]];
		}
		virus[c].symptom[rand_smp[i]].researched = false;
	}
	rand_smp.~vector();

	//Обновление вируса в зараженном городе
	town[t2].virus = &virus[c];
	town[t2].probe_taken = false;
}

void go_town()
{
	int c, d, n;
	cout << "\nВ какое поселение вы отправитесь сегодня?\n";
	cout << "1." << town[0].name << endl << "2." << town[1].name << endl
		<< "3." << town[2].name << endl << "4." << town[3].name << endl
		<< "5." << town[4].name << endl;

	do
	{
		c = input();
		if (c < 1 || c > 5)
		{
			cout << "Извините, но такого поселения не существует. Выберите одно из предложенных поселений.\n";
			continue;
		}
		if (town[c - 1].t_dead)
		{
			cout << "Данное поселение вымерло. Выберите поселение, которому еще можно чем-то помочь.\n";
		}
	} while (c < 1 || c > 5 || town[c - 1].t_dead);
	//Декрементация переменной с для работы с массивом
	c--;

	cout << "\nВы отправились в " << town[c].name << endl;

	cout << "Местные жители жалуются на '" + town[c].active_symptom.name + "'" << endl;
	if (!town[c].virus->symptom[town[c].active_symptom.lvl - 1].researched)
	{
		town[c].virus->symptom[town[c].active_symptom.lvl - 1].researched = true; //Симптом, на который жалуются крокеры, становится известным
		check_vir_research(*town[c].virus);
	}
	cout << "Вы можете:\n1.Применить лекарство.\n";
	if (town[c].quarantine)
	{
		cout << "2.Снять карантин.\n";
	}
	else
	{
		cout << "2.Закрыть поселение на карантин.\n";
	}
	if (!town[c].probe_taken)
	{
		cout << "3.Взять образец штамма.\n";
		n = 4;
	}
	else n = 3;

	do
	{
		d = input();
		if (d < 1 || d > n) cout << "Извините, но вам нельзя гладить аборигенов. Выберите одно из предложенных действий.\n";
	} while (d < 1 || d > n);
	switch (d)
	{
	case 1:
		use_drug(c);
		break;
	case 3:
		take_probe(c);
		break;
	case 2:
		set_quarantine(c);
		break;
	}
}
//Конец игровых функций


//Основные функции
void welcome_screen()
{
	cout << "\n----------XENO----------\n\n";
	cout << "Добро пожаловать на планету Инон!\n";
	cout << "Вы врач волонтерской организации 'Врачи вне парсеков'.\nВаша задача - спасти местных аборигенов.\n";
	cout << "У Вас имеется всего 90 дней до уничтожения планеты Инон! \nНа Ковчег можно поместить только полностью вылеченные поселения.\n";
	cout << "Если количество жителей поселения упадет ниже 50 особей, эвакуация \nданного поселения будет нецелесообразной, а само поселение будет считаться вымершим.\n";
	cout << "Постарайтесь любой ценой не допустить этого!\n\n";
	cout << "Краткая сводка о поселениях на планете:\n\n";
	for (int i = 0; i < 5; i++)
	{
		cout << town[i].name << " (" << town[i].p_amount << ")" << ": зараженных неизвестным вирусом " << town[i].p_infected << ", что составляет "
			<< (double)town[i].p_infected*100. / town[i].p_amount << "% от численности поселения\n";
	}
	cout << "\n(?)Подсказка: введите 'help' или 'помощь' чтобы узнать больше о командах.\n";
	cout << "\n----------Удачи!----------\n\n";
}

void game()
{
	int c, check_loose, check_win;
	while (day < 90)
	{
		cout << "\n\n----------День " << day << "----------\n\n";

		//Мутация каждые 30 дней
		if (day % 30 == 0) mutate();

		//Проверка на синтезирующиеся в данный момент лекарства и их прогресс
		synth_day();

		//Инфецирование
		srand(static_cast<unsigned int>(time(0)));
		for (int i = 0; i < 5; i++)
		{
			if (!town[i].t_dead)
			{
				town[i].infect(day);
			}
		}

		//Проверка на проигрыш
		bool one_save = false;
		check_loose = 0;
		for (int i = 0; i < 5; i++)
		{
			if (town[i].t_dead || town[i].p_infected < 3) ++check_loose;
			if (town[i].p_infected < 3) one_save = true;
		}
		if (one_save) win = true;
		if (check_loose >= 5) break;


		//Ежедневный выбор
		cout << "\nЧем займемся сегодня?\n";
		cout << "1.Обход поселений.\n2.Исследования.\n3.Синтез лекарства.\n";
		do
		{
			c = input();
			if (c < 1 || c > 3) cout << "Выберите одно из предложенных действий.\n";
		} while (c < 1 || c > 3);

		switch (c)
		{
		case 1: go_town(); break;
		case 2: research(); break;
		case 3: synth(); break;
		default: cout << "Вы провели этот день впустую.\n"; break;
		}

		++day;
	}
}

void game_over()
{
	if (win)
	{
		cout << "(*)Ковчеги укомплектованы и отправлены в зоны резервации!\n\n";
		cout << "----------УСПЕХ----------\n\n";
	}
	else
	{
		cout << "(*)Увы, но все поселения на планете Инон вымерли. Вам не удалось сохранить популяцию крокеров.\n\n";
		cout << "----------МИССИЯ ПРОВАЛЕНА----------\n\n";
	}
	cout << "Немного статистики:\n- Дней затрачено: " << day << endl;
	int p_reserved = 0, dead_towns = 0;
	for (int i = 0; i < 5; i++)
	{
		if (town[i].t_dead)
		{
			++dead_towns;
		}
		else
		{
			p_reserved += town[i].p_amount;
		}
	}
	cout << "- Крокеров спасено: " << p_reserved << endl;
	cout << "- Поселений вымерло: " << dead_towns << endl << endl;
	cout << "-----ОЦЕНКА ВАШИХ ДЕЙСТВИЙ-----" << endl;
	if (p_reserved >= 50000)
	{
		cout << "НЕВЕРОЯТНО!" << endl;
		return;
	}
	if (p_reserved >= 40000)
	{
		cout << "Отлично!" << endl;
		return;
	}
	if (p_reserved >= 30000)
	{
		cout << "Неплохо." << endl;
		return;
	}
	if (p_reserved >= 20000)
	{
		cout << "Могло быть лучше" << endl;
		return;
	}
	if (p_reserved >= 10000)
	{
		cout << "Удручающая картина..." << endl;
		return;
	}
	if (p_reserved >= 5000)
	{
		cout << "Ужасно!" << endl;
		return;
	}
	if (p_reserved >= 1000)
	{
		cout << "ЭТО КАТАСТРОФА ДЛЯ ЦЕЛОГО ВИДА!" << endl;
		return;
	}
	cout << "Вы уволены." << endl;
		
}

int main()
{
	setlocale(LC_ALL, "Russian");

	if (initialisation())
	{
		welcome_screen();
		game();
		game_over();
	}
	else cout << "Во время загрузки произошли ошибки. Перезапустите игру.\n";

	system("pause");
	return 0;
}
//Конец основных функций