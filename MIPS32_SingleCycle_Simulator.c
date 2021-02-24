#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_WORD_SIZE 500
#define MAX_INSTRUCTIONS 10000
#define MAX_INSTRUCTION_LENGTH 1000
#define MAX_LABELS 1000
#define MAX_MEMORY_SIZE 100000
#define MAX_REG_LENGTH 150

typedef struct {
	int address;
	int contents;
}memory_slot;

typedef struct {
	char name[MAX_WORD_SIZE];
	int instruction;
}label;

static memory_slot memory[MAX_MEMORY_SIZE];
static int memory_slots_used = -1, i, current_cycle,pc_monitor, write_data_reg, read_data1,read_data2,alu_out,address_monitor,write_data_memory,memory_out,regdst,jump,branch,memread,memtoreg,aluop,memwrite,alusrc,regwrite;
static int zero, pc, r0, at, v0, v1, a0, a1, a2, a3, t0, t1, t2, t3, t4, t5, t6, t7, s0, s1, s2, s3, s4, s5, s6, s7, t8, t9, k0, k1, gp, sp, fp, ra;
static int k, instruction_counter = -1, label_counter = -1, current_instruction = 0, end = 0, cycle1, cycle2, branch_instruction;
static int immediate;
static char ch, word[MAX_WORD_SIZE], instructions[MAX_INSTRUCTIONS][MAX_INSTRUCTION_LENGTH], opcode[10], rs[MAX_REG_LENGTH], rt[MAX_REG_LENGTH], rd[MAX_REG_LENGTH], read_reg1[MAX_REG_LENGTH], read_reg2[MAX_REG_LENGTH], write_reg[MAX_REG_LENGTH],branch_label[MAX_WORD_SIZE],instruction_monitor[MAX_INSTRUCTION_LENGTH];
static label labels[MAX_LABELS];
FILE* output;

int add(int a, int b) {
	return (a + b);
}

int addi(int a, int immediate) {
	return (a + immediate);
}

int addiu(int a, int immediate) {
	return (a + abs(immediate));
}

int addu(int a, int b) {
	return (a + abs(b));
}

int and (int a, int b) {
	return (a & b);
}

int andi (int a, int immediate) {
	return (a & immediate);
}

bool beq(int a, int b) {
	if (a == b)
		return true;
	else
		return false;
}

bool bne(int a, int b) {
	if (a != b)
		return true;
	else
		return false;
}

int j(int address) {
	return (address);
}

long lw(int address, int offset) {
	if (current_cycle == 67) {
		current_cycle = 67;
	}
	address = address + offset;
	int o;
	for (o = 0; o <= memory_slots_used; o++) {
		if (memory[o].address == address) {
			return memory[o].contents;
		}
	}
}

int nor(int a, int b) {
	return (~(a | b));
}

int or(int a, int b) {
	return (a | b);
}

int ori (int a, int immediate) {
	return (a | immediate);
}

int slt(int a, int b) {
	if (a < b)
		return 1;
	else
		return 0;
}

int slti(int a, int immediate) {
	if (a < immediate)
		return 1;
	else
		return 0;
}

int sltiu(int a, int immediate) {
	if (a < abs(immediate))
		return 1;
	else
		return 0;
}

int sltu(int a, int b) {
	if (a < abs(b))
		return 1;
	else
		return 0;
}

int sll(int a, int amount) {
	return (a << amount);
}

int srl(int a, int amount) {
	return (a >> amount);
}

void sw(int value, int address, int offset) {
	address = address + offset;
	int p, found=0;
	for (p = 0; p <= memory_slots_used; p++) {
		if (memory[p].address == address) {
			found = 1;
			memory[p].contents = value;
			break;
		}
	}
	if (found == 0) {
		memory_slots_used++;
		memory[memory_slots_used].address = address;
		memory[memory_slots_used].contents = value;
	}
}

int sub(int a, int b) {
	return (a - b);
}

int subu(int a, int b) {
	return (a - abs(b));
}

void swap(int a, int b) {
	memory_slot temp;
	temp.address = memory[a].address;
	temp.contents = memory[a].contents;
	memory[a].address = memory[b].address;
	memory[a].contents = memory[b].contents;
	memory[b].address = temp.address;
	memory[b].contents = temp.contents;
}

void sortmemory() {
	bool swapped=true;
	int u;
	while (swapped == true) {
		swapped = false;
		for (u = 0; i <= memory_slots_used; u++) {
			if (memory[u].address > memory[u + 1].address) {
				swap(u, u + 1);
				swapped = true;
			}
		}
	}
}

int register_decode(char reg[MAX_REG_LENGTH]) {
	if (strcmp(reg, "$zero") == 0) return 0;
	else if (strcmp(reg, "$pc") == 0) return 1;
	else if (strcmp(reg, "$r0") == 0) return 2;
	else if (strcmp(reg, "$at") == 0) return 3;
	else if (strcmp(reg, "$v0") == 0) return 4;
	else if (strcmp(reg, "$v1") == 0) return 5;
	else if (strcmp(reg, "$a0") == 0) return 6;
	else if (strcmp(reg, "$a1") == 0) return 7;
	else if (strcmp(reg, "$a2") == 0) return 8;
	else if (strcmp(reg, "$a3") == 0) return 9;
	else if (strcmp(reg, "$t0") == 0) return 10;
	else if (strcmp(reg, "$t1") == 0) return 11;
	else if (strcmp(reg, "$t2") == 0) return 12;
	else if (strcmp(reg, "$t3") == 0) return 13;
	else if (strcmp(reg, "$t4") == 0) return 14;
	else if (strcmp(reg, "$t5") == 0) return 15;
	else if (strcmp(reg, "$t6") == 0) return 16;
	else if (strcmp(reg, "$t7") == 0) return 17;
	else if (strcmp(reg, "$s0") == 0) return 18;
	else if (strcmp(reg, "$s1") == 0) return 19;
	else if (strcmp(reg, "$s2") == 0) return 20;
	else if (strcmp(reg, "$s3") == 0) return 21;
	else if (strcmp(reg, "$s4") == 0) return 22;
	else if (strcmp(reg, "$s5") == 0) return 23;
	else if (strcmp(reg, "$s6") == 0) return 24;
	else if (strcmp(reg, "$s7") == 0) return 25;
	else if (strcmp(reg, "$t8") == 0) return 26;
	else if (strcmp(reg, "$t9") == 0) return 27;
	else if (strcmp(reg, "$k0") == 0) return 28;
	else if (strcmp(reg, "$k1") == 0) return 29;
	else if (strcmp(reg, "$gp") == 0) return 30;
	else if (strcmp(reg, "$sp") == 0) return 31;
	else if (strcmp(reg, "$fp") == 0) return 32;
	else if (strcmp(reg, "$ra") == 0) return 33;
}

int register_read(char reg[MAX_REG_LENGTH]) {
	if (strcmp(reg, "$zero") == 0) return zero;
	else if (strcmp(reg, "$pc") == 0) return pc;
	else if (strcmp(reg, "$r0") == 0) return r0;
	else if (strcmp(reg, "$at") == 0) return at;
	else if (strcmp(reg, "$v0") == 0) return v0;
	else if (strcmp(reg, "$v1") == 0) return v1;
	else if (strcmp(reg, "$a0") == 0) return a0;
	else if (strcmp(reg, "$a1") == 0) return a1;
	else if (strcmp(reg, "$a2") == 0) return a2;
	else if (strcmp(reg, "$a3") == 0) return a3;
	else if (strcmp(reg, "$t0") == 0) return t0;
	else if (strcmp(reg, "$t1") == 0) return t1;
	else if (strcmp(reg, "$t2") == 0) return t2;
	else if (strcmp(reg, "$t3") == 0) return t3;
	else if (strcmp(reg, "$t4") == 0) return t4;
	else if (strcmp(reg, "$t5") == 0) return t5;
	else if (strcmp(reg, "$t6") == 0) return t6;
	else if (strcmp(reg, "$t7") == 0) return t7;
	else if (strcmp(reg, "$s0") == 0) return s0;
	else if (strcmp(reg, "$s1") == 0) return s1;
	else if (strcmp(reg, "$s2") == 0) return s2;
	else if (strcmp(reg, "$s3") == 0) return s3;
	else if (strcmp(reg, "$s4") == 0) return s4;
	else if (strcmp(reg, "$s5") == 0) return s5;
	else if (strcmp(reg, "$s6") == 0) return s6;
	else if (strcmp(reg, "$s7") == 0) return s7;
	else if (strcmp(reg, "$t8") == 0) return t8;
	else if (strcmp(reg, "$t9") == 0) return t9;
	else if (strcmp(reg, "$k0") == 0) return k0;
	else if (strcmp(reg, "$k1") == 0) return k1;
	else if (strcmp(reg, "$gp") == 0) return gp;
	else if (strcmp(reg, "$sp") == 0) return sp;
	else if (strcmp(reg, "$fp") == 0) return fp;
	else if (strcmp(reg, "$ra") == 0) return ra;
}

void register_write(char reg[MAX_REG_LENGTH], int value) {
	if (strcmp(reg, "$zero") == 0) zero = value;
	else if (strcmp(reg, "$pc") == 0) pc = value;
	else if (strcmp(reg, "$r0") == 0) r0 = value;
	else if (strcmp(reg, "$at") == 0) at = value;
	else if (strcmp(reg, "$v0") == 0) v0 = value;
	else if (strcmp(reg, "$v1") == 0) v1 = value;
	else if (strcmp(reg, "$a0") == 0) a0 = value;
	else if (strcmp(reg, "$a1") == 0) a1 = value;
	else if (strcmp(reg, "$a2") == 0) a2 = value;
	else if (strcmp(reg, "$a3") == 0) a3 = value;
	else if (strcmp(reg, "$t0") == 0) t0 = value;
	else if (strcmp(reg, "$t1") == 0) t1 = value;
	else if (strcmp(reg, "$t2") == 0) t2 = value;
	else if (strcmp(reg, "$t3") == 0) t3 = value;
	else if (strcmp(reg, "$t4") == 0) t4 = value;
	else if (strcmp(reg, "$t5") == 0) t5 = value;
	else if (strcmp(reg, "$t6") == 0) t6 = value;
	else if (strcmp(reg, "$t7") == 0) t7 = value;
	else if (strcmp(reg, "$s0") == 0) s0 = value;
	else if (strcmp(reg, "$s1") == 0) s1 = value;
	else if (strcmp(reg, "$s2") == 0) s2 = value;
	else if (strcmp(reg, "$s3") == 0) s3 = value;
	else if (strcmp(reg, "$s4") == 0) s4 = value;
	else if (strcmp(reg, "$s5") == 0) s5 = value;
	else if (strcmp(reg, "$s6") == 0) s6 = value;
	else if (strcmp(reg, "$s7") == 0) s7 = value;
	else if (strcmp(reg, "$t8") == 0) t8 = value;
	else if (strcmp(reg, "$t9") == 0) t9 = value;
	else if (strcmp(reg, "$k0") == 0) k0 = value;
	else if (strcmp(reg, "$k1") == 0) k1 = value;
	else if (strcmp(reg, "$gp") == 0) gp = value;
	else if (strcmp(reg, "$sp") == 0) sp = value;
	else if (strcmp(reg, "$fp") == 0) fp = value;
	else if (strcmp(reg, "$ra") == 0) ra = value;
}

int hexToDec(char hexVal[]){
	int len = strlen(hexVal);
	// Initializing base value to 1, i.e 16^0 
	int base = 1;
	int dec_val = 0;
	// Extracting characters as digits from last character 
	for (int i = len - 1; i >= 0; i--){
		// if character lies in '0'-'9', converting  
		// it to integral 0-9 by subtracting 48 from 
		// ASCII value. 
		if (hexVal[i] >= '0' && hexVal[i] <= '9'){
			dec_val += (hexVal[i] - 48) * base;
			// incrementing base by power 
			base = base * 16;
		}
		// if character lies in 'A'-'F' , converting  
		// it to integral 10 - 15 by subtracting 55  
		// from ASCII value 
		else if (hexVal[i] >= 'A' && hexVal[i] <= 'F'){
			dec_val += (hexVal[i] - 55) * base;
			// incrementing base by power 
			base = base * 16;
		}
	}
	return dec_val;
}

int label_decode(char labell[MAX_WORD_SIZE]) {
	for (int k = 0; k < MAX_LABELS; k++) {
		if (strcmp(labels[k].name, labell) == 0) {
			return labels[k].instruction;
			break;
		}
	}
	printf("\nERROR: Wrong Label (%s)\n",labell);
}

void print_registers_hex() {
	printf("\nREGISTERS: (Hex)\n\n");
	printf("\nPC= %X\tr0= %X\tat= %X\tv0= %X\tv1= %X\ta0= %X\t", pc, r0, at, v0, v1, a0);
	printf("\na1= %X\ta2= %X\ta3= %X\tt0= %X\tt1= %X\tt2= %X\t", a1, a2, a3, t0, t1, t2);
	printf("\nt3= %X\tt4= %X\tt5= %X\tt6= %X\tt7= %X\ts0= %X\t", t3, t4, t5, t6, t7, s0);
	printf("\ns1= %X\ts2= %X\ts3= %X\ts4= %X\ts5= %X\ts6= %X\t", s1, s2, s3, s4, s5, s6);
	printf("\ns7= %X\tt8= %X\tt9= %X\tk0= %X\tk1= %X\tgp= %X\t", s7, t8, t9, k0, k1, gp);
	printf("\nsp= %X\tfp= %X\tra= %X", sp, fp, ra);
}

void print_registers_dec() {
	printf("\nREGISTERS: (Dec)\n\n");
	printf("\nPC= %d\tr0= %d\tat= %d\tv0= %d\tv1= %d\ta0= %d\t", pc, r0, at, v0, v1, a0);
	printf("\na1= %d\ta2= %d\ta3= %d\tt0= %d\tt1= %d\tt2= %d\t", a1, a2, a3, t0, t1, t2);
	printf("\nt3= %d\tt4= %d\tt5= %d\tt6= %d\tt7= %d\ts0= %d\t", t3, t4, t5, t6, t7, s0);
	printf("\ns1= %d\ts2= %d\ts3= %d\ts4= %d\ts5= %d\ts6= %d\t", s1, s2, s3, s4, s5, s6);
	printf("\ns7= %d\tt8= %d\tt9= %d\tk0= %d\tk1= %d\tgp= %d\t", s7, t8, t9, k0, k1, gp);
	printf("\nsp= %d\tfp= %d\tra= %d", sp, fp, ra);
}

void print_memory() {
	printf("\t\tMEMORY\n\n");
	for (int w = 0; w <= memory_slots_used; w++) {
		printf("Address: %X\t\tContents (Decimal): %d\tContents (Hex): %X\n", memory[w].address, memory[w].contents, memory[w].contents);
	}
}

void export_cycle_info(int cycle) {
	sortmemory();
	fprintf(output, "-----Cycle %d-----\nRegisters:\n", cycle);
	fprintf(output, "%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x", pc, r0, at, v0, v1, a0, a1, a2, a3, t0, t1, t2, t3, t4, t5, t6, t7, s0, s1, s2, s3, s4, s5, s6, s7, t8, t9, k0, k1, gp, sp, fp, ra);
	fprintf(output, "\n\nMonitors:\n");
	fprintf(output, "%x\t%s\t%s\t%s\t%s", pc_monitor, instruction_monitor,read_reg1,read_reg2,write_reg);
	if (write_data_reg != -1)
		fprintf(output, "\t%x", write_data_reg);
	else
		fprintf(output, "\t-");
	if (read_data1 != -1)
		fprintf(output, "\t%x", read_data1);
	else
		fprintf(output, "\t-");
	if (read_data2 != -1)
		fprintf(output, "\t%x", read_data2);
	else
		fprintf(output, "\t-");
	if (alu_out != -1)
		fprintf(output, "\t%x", alu_out);
	else
		fprintf(output, "\t-");
	if (branch_label[0] != '\0')
		fprintf(output, "\t%s", branch_label);
	else
		fprintf(output, "\t-");
	if (address_monitor != -1)
		fprintf(output, "\t%x", address_monitor);
	else
		fprintf(output, "\t-");
	if (write_data_memory != -1)
		fprintf(output, "\t%x", write_data_memory);
	else
		fprintf(output, "\t-");
	if (memory_out != -1)
		fprintf(output, "\t%x", memory_out);
	else
		fprintf(output, "\t-");
	fprintf(output, "\t%d\t%d\t%d\t%d\t%d\t%02d\t%d\t%d\t%d", regdst, jump, branch, memread, memtoreg, aluop, memwrite, alusrc, regwrite);
	fprintf(output, "\n\nMemory State:\n");
	for (int r = 0; r <= memory_slots_used; r++) {
		if (r != memory_slots_used) //gia na min tiponete extra tab sto telos tin grammis
			fprintf(output, "%x\t", memory[r].contents);
		else
			fprintf(output, "%x", memory[r].contents);
	}
	fprintf(output, "\n\n");
}

void export_final_info() {
	sortmemory();
	fprintf(output, "-----Final State-----\nRegisters:\n");
	fprintf(output, "%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x", pc, r0, at, v0, v1, a0, a1, a2, a3, t0, t1, t2, t3, t4, t5, t6, t7, s0, s1, s2, s3, s4, s5, s6, s7, t8, t9, k0, k1, gp, sp, fp, ra);
	fprintf(output, "\n\nMemory State:\n");
	for (int r = 0; r <= memory_slots_used; r++) {
		if (r != memory_slots_used) //gia na min tiponete extra tab sto telos tin grammis
			fprintf(output, "%x\t", memory[r].contents);
		else
			fprintf(output, "%x", memory[r].contents);
	}
	fprintf(output, "\n\nTotal Cycles:\n%d",current_cycle);
	fclose(output);
}

int main() {
	FILE* filepointer;
	filepointer = fopen("find_min_test.s", "r");
	//Initializaton of values
	for (i = 0; i < MAX_MEMORY_SIZE; i++) {
		memory[i].address = 0;
	}
	gp = 0x10008000;
	sp = 0x7ffffffc;
	zero = 0;
	//Reading the file (Parser)
	while ((ch = getc(filepointer)) != EOF) {
		switch (ch) {
		case '.':
			fscanf(filepointer, "%s", word);
			if (strcmp(word, "data") == 0) { //memory begins here
				break;
			}
			if (strcmp(word, "text") == 0) { //code begins here
				fscanf(filepointer, "%s", word);
				while (word != EOF) {
					switch (word[0]) {
					case '\n':
						fscanf(filepointer, "%s", word);
						break;
					case '#': //ignore comment line
						while ((ch = getc(filepointer)) != '\n') {

						}
						fscanf(filepointer, "%s", word);
						break;
					default: //this line contains "main:", a comment or a label.
						if (word[0] == '#') { //ignore comment line

							break;
						}
						if (strchr(word, ':') != NULL) { //if this is true, it's a label
							label_counter++;
							labels[label_counter].instruction = instruction_counter + 1;
							i = 0;
							while (word[i] != ':') { //copy the label name
								labels[label_counter].name[i] = word[i];
								i++;
							}
							labels[label_counter].name[i] = '\0';
							printf("\n\nLabel %d is %s and points to instruction %d.\n", label_counter, labels[label_counter].name, labels[label_counter].instruction);
							fscanf(filepointer, "%s", word);
							break;
						}
						//else, it's an instruction
						i = 0;
						instruction_counter++;
						while (word[i] != '\0') { //copy the first word of the instruction
							instructions[instruction_counter][i] = word[i];
							i++;
						}
						while ((ch = getc(filepointer)) != '\n') {
							if ((ch == '#') || (ch == '\t')) { //if we find a comment, ignore it
								while (((ch = getc(filepointer)) != '\n') && (ch != EOF)) {
								}
								break;
							}
							if (ch == EOF) { //if we find the end of file
								break;
							}
							instructions[instruction_counter][i] = ch;
							i++;
						}
						instructions[instruction_counter][i] = '\0';
						printf("\nInstruction %d is: %s", instruction_counter, instructions[instruction_counter]);
						fscanf(filepointer, "%s", word);
						break;
					}
					if (ch == EOF) { //if we find the end of file
						break;
					}
				}
				if (ch == EOF) { //if we find the end of file
					break;
				}
			}
			break;
		case '#': //ignore comment line
			while ((ch = getc(filepointer)) != '\n') {

			}
			break;
		case ('\t' || ' '):
			ch = getc(filepointer);
			break;
		}
		if (ch == EOF) { //if we find the end of file
			break;
		}
	}
	fclose(filepointer);
	output = fopen("output_file.txt", "w");
	fprintf(output, "Name: Michalis Piponidis\nID: 912526\n\n");
	printf("\nInsert Cycle 1: ");
	scanf("%d", &cycle1);
	printf("\nInsert Cycle 2: ");
	scanf("%d", &cycle2);
	instruction_counter--;
	//Execution of Instructions
	current_cycle = 0;
	while (end == 0) { //end becomes 1 when sll $zero, $zero, 0 is found
		current_cycle++;
		//if (current_cycle == 34) {
		//	current_cycle = 34;
		//}
		i = 0;
		while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\t')) { //isolate the opcode (instruction)
			opcode[i] = instructions[current_instruction][i];
			i++;
		}
		opcode[i] = '\0';
		if (strcmp(opcode, "add") == 0) { //add
			
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			register_write(rd, add(register_read(rs), register_read(rt)));
			printf("\nInstruction %d is %s with rd= %s (%d) rs= %s (%d) rt= %s (%d)\n", current_instruction, opcode, rd, register_decode(rd), rs, register_decode(rs), rt, register_decode(rt));
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 10;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
		}
		else if (strcmp(opcode, "addi") == 0) { //addi
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto immediate
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to immediate
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			if ((rd[0] == '0') && (rd[1] == 'x'))
				immediate = hexToDec(rd);
			else
				immediate = atoi(rd);
			register_write(rt, addi(register_read(rs), immediate));
			printf("\nInstruction %d is %s with rs= %s (%d) rt= %s (%d) immediate= %d\n", current_instruction, opcode, rs, register_decode(rs), rt, register_decode(rt), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, "-");
			strcpy(write_reg, rt);
			write_data_reg = register_read(rt);
			read_data1 = register_read(rs);
			read_data2 = -1;
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 0;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 10;
			memwrite = 0;
			alusrc = 1;
			regwrite = 1;
		}
		else if (strcmp(opcode, "addiu") == 0) { //addiu
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto immediate
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to immediate
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			if ((rd[0] == '0') && (rd[1] == 'x'))
				immediate = hexToDec(rd);
			else
				immediate = atoi(rd);
			register_write(rs, addiu(register_read(rt), immediate));
			printf("\nInstruction %d is %s with rs= %s (%d) rt= %s (%d) immediate= %d\n", current_instruction, opcode, rs, register_decode(rs), rt, register_decode(rt), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, "-");
			strcpy(write_reg, rt);
			write_data_reg = register_read(rt);
			read_data1 = register_read(rs);
			read_data2 = -1;
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 0;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 10;
			memwrite = 0;
			alusrc = 1;
			regwrite = 1;
		}
		else if (strcmp(opcode, "addu") == 0) { //addu
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			register_write(rd, addu(register_read(rs), register_read(rt)));
			printf("\nInstruction %d is %s with rd= %s (%d) rs= %s (%d) rt= %s (%d)\n", current_instruction, opcode, rd, register_decode(rd), rs, register_decode(rs), rt, register_decode(rt));
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 10;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
		}
		else if (strcmp(opcode, "and") == 0) { //and
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			register_write(rd, and (register_read(rs), register_read(rt)));
			printf("\nInstruction %d is %s with rd= %s (%d) rs= %s (%d) rt= %s (%d)\n", current_instruction, opcode, rd, register_decode(rd), rs, register_decode(rs), rt, register_decode(rt));
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 00;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
}
		else if (strcmp(opcode, "andi") == 0) { //andi
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto immediate
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to immediate
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			if ((rd[0] == '0') && (rd[1] == 'x'))
				immediate = hexToDec(rd);
			else
				immediate = atoi(rd);
			register_write(rt, andi(register_read(rs), immediate));
			printf("\nInstruction %d is %s with rs= %s (%d) rt= %s (%d) immediate= %d\n", current_instruction, opcode, rs, register_decode(rs), rt, register_decode(rt), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, "-");
			strcpy(write_reg, rt);
			write_data_reg = register_read(rt);
			read_data1 = register_read(rs);
			read_data2 = -1;
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 0;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 00;
			memwrite = 0;
			alusrc = 1;
			regwrite = 1;
}
		else if (strcmp(opcode, "beq") == 0) { //beq
		strcpy(instruction_monitor, instructions[current_instruction]); //Prepei na parume to monitor apo prin edw epeidi pithanon na allaksi i entoli
		pc_monitor = current_instruction * 4; //Prepei na parume to monitor apo prin edw epeidi pithanon na allaksi i entoli
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto label
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to label
				word[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			word[k] = '\0';
			branch_instruction = label_decode(word);
			if (beq(register_read(rs), register_read(rt))) {
				current_instruction = branch_instruction-1; //-1 epeidi kanume +1 sto telos tou loop
			}
			printf("\nInstruction %d is %s with rs= %s (%d) rt= %s (%d) label= %s. Branch: %d\n", current_instruction, opcode, rs, register_decode(rs), rt, register_decode(rt), word, (register_read(rs) == register_read(rt)));
			//MONITOR ASSIGNMENT
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, "-");
			write_data_reg = -1;
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = register_read(rs) - register_read(rt);
			strcpy(branch_label, word);
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 0;
			jump = 0;
			branch = 1;
			memread = 0;
			memtoreg = 0;
			aluop = 10;
			memwrite = 0;
			alusrc = 0;
			regwrite = 0;
}
		else if (strcmp(opcode, "bne") == 0) { //bne
		strcpy(instruction_monitor, instructions[current_instruction]); //Prepei na parume to monitor apo prin edw epeidi pithanon na allaksi i entoli
		pc_monitor = current_instruction * 4; //Prepei na parume to monitor apo prin edw epeidi pithanon na allaksi i entoli
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto label
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to label
				word[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			word[k] = '\0';
			branch_instruction = label_decode(word);
			if (bne(register_read(rs),register_read(rt))) {
				current_instruction = branch_instruction-1; //-1 epeidi kanume +1 sto telos tou loop
			}
			printf("\nInstruction %d is %s with rs= %s (%d) rt= %s (%d) label= %s. Branch: %d\n", current_instruction, opcode, rs, register_decode(rs), rt, register_decode(rt), word, (register_read(rs) != register_read(rt)));
			//MONITOR ASSIGNMENT
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, "-");
			write_data_reg = -1;
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = register_read(rs) - register_read(rt);
			strcpy(branch_label, word);
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 0;
			jump = 0;
			branch = 1;
			memread = 0;
			memtoreg = 0;
			aluop = 10;
			memwrite = 0;
			alusrc = 0;
			regwrite = 0;
}
		else if (strcmp(opcode, "j") == 0) { //j
		strcpy(instruction_monitor, instructions[current_instruction]); //Prepei na parume to monitor apo prin edw epeidi pithanon na allaksi i entoli
		pc_monitor = current_instruction * 4; //Prepei na parume to monitor apo prin edw epeidi pithanon na allaksi i entoli
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto label
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to label
				word[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			word[k] = '\0';
			current_instruction = label_decode(word)-1; //-1 epeidi kanume +1 sto telos tou loop
			printf("\nInstruction %d is %s with label= %s. Current Instruction = %d\n", current_instruction, opcode, word, current_instruction);
			//MONITOR ASSIGNMENT
			strcpy(read_reg1, "-");
			strcpy(read_reg2, "-");
			strcpy(write_reg, "-");
			write_data_reg = -1;
			read_data1 = -1;
			read_data2 = -1;
			alu_out = -1;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 0;
			jump = 1;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 00;
			memwrite = 0;
			alusrc = 0;
			regwrite = 0;
}
		else if (strcmp(opcode, "lw") == 0) { //lw
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto offset
				i++;
			}
			k = 0;
			while (instructions[current_instruction][i] != '(') { //diavazume to offset
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			if ((rs[0] == '0') && (rs[1] == 'x'))
				immediate = hexToDec(rs); //offset
			else
				immediate = atoi(rs); //offset
			i++; //gia na ftasoume sto base
			k = 0;
			while (instructions[current_instruction][i] != ')') { //diavazume to base
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			register_write(rt, lw(register_read(rd), immediate));
			printf("\nInstruction %d is %s with rt= %s (%d) base= %s (%d) offset= %d.\n", current_instruction, opcode, rt, register_decode(rt), rd, register_decode(rd), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rd);
			strcpy(read_reg2, "-");
			strcpy(write_reg, rt);
			write_data_reg = register_read(rt);
			read_data1 = register_read(rd);
			read_data2 = -1;
			alu_out = immediate + register_read(rd);
			branch_label[0] = '\0';
			address_monitor = alu_out;
			write_data_memory = -1;
			memory_out = register_read(rt);
			regdst = 0;
			jump = 0;
			branch = 0;
			memread = 1;
			memtoreg = 1;
			aluop = 10;
			memwrite = 0;
			alusrc = 1;
			regwrite = 1;
}
		else if (strcmp(opcode, "nor") == 0) { //nor
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			register_write(rd, nor(register_read(rs), register_read(rt)));
			printf("\nInstruction %d is %s with rd= %s (%d) rs= %s (%d) rt= %s (%d)\n", current_instruction, opcode, rd, register_decode(rd), rs, register_decode(rs), rt, register_decode(rt));
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 00;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
}
		else if (strcmp(opcode, "or") == 0) { //or
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			register_write(rd, or (register_read(rs), register_read(rt)));
			printf("\nInstruction %d is %s with rd= %s (%d) rs= %s (%d) rt= %s (%d)\n", current_instruction, opcode, rd, register_decode(rd), rs, register_decode(rs), rt, register_decode(rt));
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 01;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
}
		else if (strcmp(opcode, "ori") == 0) { //ori
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto immediate
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to immediate
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			if ((rd[0] == '0') && (rd[1] == 'x'))
				immediate = hexToDec(rd);
			else
				immediate = atoi(rd);
			register_write(rt, ori(register_read(rs), immediate));
			printf("\nInstruction %d is %s with rs= %s (%d) rt= %s (%d) immediate= %d\n", current_instruction, opcode, rs, register_decode(rs), rt, register_decode(rt), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, "-");
			strcpy(write_reg, rt);
			write_data_reg = register_read(rt);
			read_data1 = register_read(rs);
			read_data2 = -1;
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 0;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 01;
			memwrite = 0;
			alusrc = 1;
			regwrite = 1;
}
		else if (strcmp(opcode, "slt") == 0) { //slt
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			register_write(rd, slt(register_read(rs), register_read(rt)));
			printf("\nInstruction %d is %s with rd= %s (%d) rs= %s (%d) rt= %s (%d)\n", current_instruction, opcode, rd, register_decode(rd), rs, register_decode(rs), rt, register_decode(rt));
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 11;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
}
		else if (strcmp(opcode, "slti") == 0) { //slti
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto immediate
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to immediate
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			if ((rd[0] == '0') && (rd[1] == 'x'))
				immediate = hexToDec(rd);
			else
				immediate = atoi(rd);
			register_write(rt, slti(register_read(rs), immediate));
			printf("\nInstruction %d is %s with rs= %s (%d) rt= %s (%d) immediate= %d\n", current_instruction, opcode, rs, register_decode(rs), rt, register_decode(rt), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, "-");
			strcpy(write_reg, rt);
			write_data_reg = register_read(rt);
			read_data1 = register_read(rs);
			read_data2 = -1;
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 0;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 11;
			memwrite = 0;
			alusrc = 1;
			regwrite = 1;
}
		else if (strcmp(opcode, "sltiu") == 0) { //sltiu
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto immediate
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to immediate
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			if ((rd[0] == '0') && (rd[1] == 'x'))
				immediate = hexToDec(rd);
			else
				immediate = atoi(rd);
			register_write(rt, sltiu(register_read(rs), immediate));
			printf("\nInstruction %d is %s with rs= %s (%d) rt= %s (%d) immediate= %d\n", current_instruction, opcode, rs, register_decode(rs), rt, register_decode(rt), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, "-");
			strcpy(write_reg, rt);
			write_data_reg = register_read(rt);
			read_data1 = register_read(rs);
			read_data2 = -1;
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 0;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 11;
			memwrite = 0;
			alusrc = 1;
			regwrite = 1;
}
		else if (strcmp(opcode, "sltu") == 0) { //sltu
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			register_write(rd, sltu(register_read(rs), register_read(rt)));
			printf("\nInstruction %d is %s with rd= %s (%d) rs= %s (%d) rt= %s (%d)\n", current_instruction, opcode, rd, register_decode(rd), rs, register_decode(rs), rt, register_decode(rt));
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 11;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
}
		else if (strcmp(opcode, "sll") == 0) { //sll
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto shift amount
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to shift amount
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			if ((rs[0] == '0') && (rs[1] == 'x'))
				immediate = hexToDec(rs);
			else
				immediate = atoi(rs);
			//Sinthiki teliomatos programmatos
			if ((strcmp("$zero", rd) == 0) && (strcmp("$zero", rt) == 0) && (immediate == 0)) {
				end = 1;
				break;
			}
			register_write(rd, sll(register_read(rt), immediate));
			printf("\nInstruction %d is %s with rd= %s (%d) rt= %s (%d) shift amount= %d\n", current_instruction, opcode, rd, register_decode(rd), rt, register_decode(rt), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, "-");
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = -1;
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 00;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
}
		else if (strcmp(opcode, "srl") == 0) { //srl
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto shift amount
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t') && (instructions[current_instruction][i] != '\0')) { //diavazume to shift amount
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			if ((rs[0] == '0') && (rs[1] == 'x'))
				immediate = hexToDec(rs);
			else
				immediate = atoi(rs);
			register_write(rd, srl(register_read(rt), immediate));
			printf("\nInstruction %d is %s with rd= %s (%d) rt= %s (%d) shift amount= %d\n", current_instruction, opcode, rd, register_decode(rd), rt, register_decode(rt), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, "-");
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = -1;
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 00;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
}
		else if (strcmp(opcode, "sw") == 0) { //sw
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			while ((instructions[current_instruction][i] == ' ') || (instructions[current_instruction][i] == ',')) { //mexri na ftasoume sto offset
				i++;
			}
			k = 0;
			while (instructions[current_instruction][i] != '(') { //diavazume to offset
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			if ((rs[0] == '0') && (rs[1] == 'x'))
				immediate = hexToDec(rs); //offset
			else
				immediate = atoi(rs); //offset
			i++; //gia na ftasoume sto base
			k = 0;
			while (instructions[current_instruction][i] != ')') { //diavazume to base
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			sw(register_read(rt), register_read(rd), immediate);
			printf("\nInstruction %d is %s with rt= %s (%d) base= %s (%d) offset= %d.\n", current_instruction, opcode, rt, register_decode(rt), rd, register_decode(rd), immediate);
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rd);
			strcpy(read_reg2, rt);
			strcpy(write_reg, "-");
			write_data_reg = -1;
			read_data1 = register_read(rd);
			read_data2 = register_read(rt);
			alu_out = immediate + register_read(rd);
			branch_label[0] = '\0';
			address_monitor = alu_out;
			write_data_memory = read_data2;
			memory_out = 0;
			regdst = 0;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 10;
			memwrite = 1;
			alusrc = 1;
			regwrite = 0;
}
		else if (strcmp(opcode, "sub") == 0) { //sub
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
				rd[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rd[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
				rs[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rs[k] = '\0';
			while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
				i++;
			}
			k = 0;
			while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t')) { //diavazume ton rt
				rt[k] = instructions[current_instruction][i];
				i++;
				k++;
			}
			rt[k] = '\0';
			register_write(rd, sub(register_read(rs), register_read(rt)));
			printf("\nInstruction %d is %s with rd= %s (%d) rs= %s (%d) rt= %s (%d)\n", current_instruction, opcode, rd, register_decode(rd), rs, register_decode(rs), rt, register_decode(rt));
			//MONITOR ASSIGNMENT
			pc_monitor = current_instruction * 4;
			strcpy(instruction_monitor, instructions[current_instruction]);
			strcpy(read_reg1, rs);
			strcpy(read_reg2, rt);
			strcpy(write_reg, rd);
			write_data_reg = register_read(rd);
			read_data1 = register_read(rs);
			read_data2 = register_read(rt);
			alu_out = write_data_reg;
			branch_label[0] = '\0';
			address_monitor = -1;
			write_data_memory = -1;
			memory_out = -1;
			regdst = 1;
			jump = 0;
			branch = 0;
			memread = 0;
			memtoreg = 0;
			aluop = 10;
			memwrite = 0;
			alusrc = 0;
			regwrite = 1;
}
		else if (strcmp(opcode, "subu") == 0) { //subu
		while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rd
			i++;
		}
		k = 0;
		while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rd
			rd[k] = instructions[current_instruction][i];
			i++;
			k++;
		}
		rd[k] = '\0';
		while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rs
			i++;
		}
		k = 0;
		while ((instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != ',')) { //diavazume ton rs
			rs[k] = instructions[current_instruction][i];
			i++;
			k++;
		}
		rs[k] = '\0';
		while (instructions[current_instruction][i] != '$') { //mexri na ftasoume ston rt
			i++;
		}
		k = 0;
		while ((instructions[current_instruction][i] != '\0') && (instructions[current_instruction][i] != ' ') && (instructions[current_instruction][i] != '\n') && (instructions[current_instruction][i] != '\t')) { //diavazume ton rt
			rt[k] = instructions[current_instruction][i];
			i++;
			k++;
		}
		rt[k] = '\0';
		register_write(rd, subu(register_read(rs), register_read(rt)));
		printf("\nInstruction %d is %s with rd= %s (%d) rs= %s (%d) rt= %s (%d)\n", current_instruction, opcode, rd, register_decode(rd), rs, register_decode(rs), rt, register_decode(rt));
		//MONITOR ASSIGNMENT
		pc_monitor = current_instruction * 4;
		strcpy(instruction_monitor, instructions[current_instruction]);
		strcpy(read_reg1, rs);
		strcpy(read_reg2, rt);
		strcpy(write_reg, rd);
		write_data_reg = register_read(rd);
		read_data1 = register_read(rs);
		read_data2 = register_read(rt);
		alu_out = write_data_reg;
		branch_label[0] = '\0';
		address_monitor = -1;
		write_data_memory = -1;
		memory_out = -1;
		regdst = 1;
		jump = 0;
		branch = 0;
		memread = 0;
		memtoreg = 0;
		aluop = 10;
		memwrite = 0;
		alusrc = 0;
		regwrite = 1;
}
		current_instruction++;
		pc = current_instruction * 4;
		if ((current_cycle == cycle1) || (current_cycle == cycle2)) {
			export_cycle_info(current_cycle);
		}
			printf("\nCYCLE %d\n\n", current_cycle);
	}
	export_final_info();
	printf("\nThe program has ended.\n");
}