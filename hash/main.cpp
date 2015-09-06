#include <iostream>
#include <locale>

typedef unsigned long hash_t;

enum HashType
{
	COMMAND = 0,
	SERVER_FILE
};

enum ActionID
{
	UNKNOWN = -1,
	SAFE = 0,
	BLOCK,
	KICK,
	BAN,
};

struct HashInfo
{
	HashType type;
	hash_t hash;
	size_t len;
	ActionID action;
	char first_char;
};

int main()
{
	unsigned long hash = 0x12345678;
	FILE* tohash = fopen("C:\\nocheatz\\server-plugin\\Res\\tohash.txt", "r");
	FILE* hfile = fopen("C:\\nocheatz\\server-plugin\\Res\\hash.dat", "wb");

	while(!feof(tohash))
	{
		hash = 0x13377FFF;
		char to_encrypt[253];
		int action;
		fscanf(tohash, "%s %d\n", &to_encrypt, &action);

		size_t len = 0;
		for(; to_encrypt[len] != '\0'; ++len)
		{
				size_t lchar;
				if(to_encrypt[len] < 'a')
				{
					lchar = (((to_encrypt[len] + ('a' - 'A')) % ('z' - 'a')) + 'a');
				}
				else
				{
					lchar = ((to_encrypt[len] % ('z' - 'a')) + 'a');
				}
				size_t y = lchar % 4;
				unsigned long chartohash = (lchar << (y*8));
				unsigned long indextohash = (len << y);
				hash ^= chartohash & ~indextohash;
		}
		std::locale loc;
		HashType type = HashType::COMMAND;
		fwrite(&type, sizeof(HashType), 1, hfile);
		fwrite(&hash, sizeof(hash_t), 1, hfile);
		fwrite(&len, sizeof(size_t), 1, hfile);
		fwrite(&action, sizeof(ActionID), 1, hfile);
		char first_char = std::tolower(to_encrypt[0], loc);
		fwrite(&first_char, sizeof(char), 1, hfile);
	}
	fclose(hfile);
	fclose(tohash);
	return 0;
}
