#pragma once
using namespace std;

class smp
{
public:
	string name;
	int lvl;
	int infect;
	int death;
	bool researched;

	virtual ~smp()
	{

	}
	virtual void init(string init_name, int init_lvl, int init_infect, int init_death, bool init_researched = false)
	{
		name = init_name;
		lvl = init_lvl;
		infect = init_infect;
		death = init_death;
		researched = init_researched;
	}
};

class vir
{
public:
	string name;
	smp symptom[4];
	bool researched;
	bool probed;

	vir()
	{
		name = "<����������� �����>";
		researched = false;
		probed = false;
	}
	virtual ~vir()
	{

	}
	bool research_check()
	{
		for (int i = 0; i < 4; i++)
		{
			if (!symptom[i].researched)
				return false;
		}
		researched = true;
		cout << "(!)����� " + name + " ���������� � ����� � ������������� ��� ������� ���������.\n";
		return true;
	}
};

class med
{
public:
	string name;
	smp GE[4];
	smp BE[4];
	int synth_day;

	med()
	{
		synth_day = 7;
	}
	virtual ~med()
	{

	}
};

class twn
{
private:
	const int CHANCE = 30;
	const int INC_DEADTH_COMBO = 1;
	int death_combo = 0; //������ ���� �� ����������� ������� ����� ��������� 1% � ����������.
	int infect_break = 0;

	bool act_smp(int p)
	{
		if (active_symptom.lvl < 4 && p >= 75)
		{
			int ch = rand() % 100;
			if (ch <= CHANCE)
			{
				active_symptom = virus->symptom[3];
				return true;
			}
		}
		if (active_symptom.lvl < 3 && p >= 50)
		{
			int ch = rand() % 100;
			if (ch <= CHANCE)
			{
				active_symptom = virus->symptom[2];
				return true;
			}
		}
		if (active_symptom.lvl < 2 && p >= 15)
		{
			int ch = rand() % 100;
			if (ch <= CHANCE)
			{
				active_symptom = virus->symptom[1];
				return true;
			}
		}
		return false;
	}

	bool deact_smp(int p)
	{
		if (active_symptom.lvl == 4 && p <= 75)
		{
			active_symptom = virus->symptom[2];
			return true;
		}
		if (active_symptom.lvl == 3 && p <= 50)
		{
			active_symptom = virus->symptom[1];
			return true;
		}
		if (active_symptom.lvl == 2 && p <= 15)
		{
			active_symptom = virus->symptom[0];
			return true;
		}
		if (p < 3)
		{
			active_symptom.~smp();
			cout << "(+)������� ������ � ��������� " + name + " ������������.\n";
			active_symptom.~smp();
			p_infected = 0;
		}
		return false;
	}

	void vir_death()
	{
		int r = rand() % active_symptom.death + death_combo;
		int new_dead = (int)(p_infected * (float)r / 100);
		p_amount -= new_dead;
		p_infected -= new_dead;
		p_dead += new_dead;
		if (new_dead > 0)
		{
			cout << "(!)� ��������� " + name + " ������ �������! (�������: " << new_dead << ")\n";
		}
		if (active_symptom.lvl == 4)
		{
			if (death_combo < 100) { death_combo += INC_DEADTH_COMBO; }
			else death_combo = 90;
		}
		else death_combo = 0;
	}

public:
	string name;
	int p_amount;
	int p_infected;
	int p_dead;
	vir *virus;
	bool quarantine;
	bool t_dead;
	smp active_symptom;
	bool probe_taken;

	virtual ~twn()
	{

	}

	virtual void init(string init_name, vir &init_virus, int init_amount = 10000, int init_infected = 500, int init_dead = 0, bool init_quarantine = false)
	{
		name = init_name;
		p_amount = init_amount;
		p_infected = init_infected;
		p_dead = init_dead;
		virus = &init_virus;
		quarantine = init_quarantine;
		active_symptom = virus->symptom[0];
		t_dead = false;
		probe_taken = false;
	}

	void infect()
	{
		if (!p_infected) return;
		int ph = p_amount - p_infected;
		int r = rand() % active_symptom.infect - infect_break;
		if (r < 0) r = 0;
		int pi = (int)(ph * (float)r / 100);
		p_infected += pi;

		//�������� �� ��������� ������ ��������
		int p = (int)((double)p_infected*100. / p_amount);
		//������� ����� ������� 100% ���������
		if (p == 99 && p_infected < p_amount)
		{
			p_infected += 1;
			if (p_infected == p_amount) cout << "(!)� ��������� " + name + " �� �������� �������� ��������!\n";
		}
		if (act_smp(p))
		{
			cout << "(!)����� �������������� � ��������� " + name + "! (" << p << "% ����������)\n";
		}

		//���������� ��������.
		if (active_symptom.death != 0)
		{
			vir_death();
		}

		if (p_amount <= 50 && t_dead == false)
		{
			t_dead = true;
			cout << "(X)��������� " + name + " �������.\n";
			name += " (�������)";
			p_amount = 0;
			p_infected = 0;
			p_dead = 10000;
		}

		if (infect_break >= 5)
		{
			infect_break -= 5;
			if (infect_break < 5 && infect_break > 0) infect_break = 0;
		}
	}

	void heal(int heal_precent, int slowdown, string h_name)
	{
		infect_break = slowdown;
		if (heal_precent != 0)
		{
			int r = rand() % heal_precent + 1;
			int p_healed = (int)(p_infected * (float)r / 100);
			if (heal_precent < 0) { p_infected += p_healed; }
			else p_infected -= p_healed;

			//�������� �� ����������� ��������
			int p = (int)((double)p_infected*100. / p_amount);

			if (heal_precent > 0)
			{
				cout << h_name + " ������������ ������������ �� �������� (" << p_healed << " �������� ��������)\n";
			}
			else if (heal_precent < 0)
			{
				cout << h_name + " ������������ ������������ �� �������� (" << p_healed << " �������� ����� ����)\n";
			}

			if (deact_smp(p))
			{
				cout << "(+)����� �������� � ��������� " + name + " (" << p << "% ����������)\n";
			}
		}
		else
		{
			cout << "\n������, " + h_name + " ���������� ������ ������ " + virus->name << endl;
		}
	}
};