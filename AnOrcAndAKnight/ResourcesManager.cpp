#include "Settings.h"
#include "ResourcesManager.h"

#include "Hero.h"
#include "UIManager.h"

map<string, string> ResourcesManager::_catLanguageResources;
vector<Hero*> ResourcesManager::_listHeroes;

string ToAscii(const string& text) {
    const char* s = text.c_str();
    char buffer[10000];
    CharToOemA(s, buffer);
    string str(buffer);
    return str;
}

vector<string> ResourcesManager::GetAvailableLanguages()
{
    ifstream languageFile("Settings/Language.csv");
    if (languageFile.is_open() == false) {
        cerr << "Unable to open file !\n";
        exit(1);
        return vector<string>();
    }

    // read first line to get all availables language
    string line;
    getline(languageFile, line);

    // tokenize the line to get all languages
    stringstream checkLanguage(line);
    string intermediate;
    vector<string> tokens;
    while (getline(checkLanguage, intermediate, ';'))
        tokens.push_back(ToAscii(intermediate));

    // remove first value ("LANGUAGE" key)
    tokens.erase(tokens.begin());

    languageFile.close();
    return tokens;
}

void ResourcesManager::LoadLanguage(int langId)
{
    ifstream languageFile("Settings/Language.csv");
    if (languageFile.is_open() == false) {
        std::cerr << "Unable to open file !";
        return;
    }

    // read first line to get position of current language
    string line;
    getline(languageFile, line);

    // get all texts in current langue with key
    vector<string> tokens;
    while (getline(languageFile, line)) {
        tokens.clear();
        stringstream texts(line);
        string s;
        while (getline(texts, s, ';')) {
            // replace "\n" by real line breaks
            size_t pos;
            while ((pos = s.find("\\n")) != string::npos)
                s.replace(pos, 2, "\n");

            // replace "\t" by real tabs
            pos = -1;
            while ((pos = s.find("\\t")) != string::npos)
                s.replace(pos, 2, "\t");
            s = ToAscii(s);
            tokens.push_back(s);
        }

        _catLanguageResources.insert({ tokens[0], tokens[langId] });
    }
    languageFile.close();
    
    HeroClass::InitClasses();
}

void ResourcesManager::LoadHeroes()
{
    ifstream heroesFile("Settings/Heroes.csv");
    if (heroesFile.is_open() == false) {
        std::cerr << "Unable to open file !";
        return;
    }

    // ignore first line (information to complete it with Excel
    string line;
    getline(heroesFile, line);

    string parseError;
    while (getline(heroesFile, line)) {
        // tokenize the line to get all distinct values
        stringstream stream(line);
        string val;
        vector<string> tokens;
        while (getline(stream, val, ';'))
            tokens.push_back(val);

        // HERO CLASS
        bool classFound = false;
        HeroClass heroClass = HeroClass::_listClasses[0];
        for (auto& hc : HeroClass::_listClasses) {
            string className = GetT(tokens[0]);
            if (hc._name != className)
                continue;

            classFound = true;
            heroClass = hc;
            break;
        }
        if(classFound == false)
            parseError += "Invalid Class Name for Hero : \"" + tokens[0] + "\" could not be found. \"" + heroClass._name + "\" used by default\n";

        // HERO NAME
        string name = ToAscii(tokens[1]);
        
        // HERO HP
        int hp = 50;
        if (UIManager::IsNumber(tokens[2]) == false || atoi(tokens[2].c_str()) == 0)
            parseError += "Invalid HP value for Hero [" + to_string(_listHeroes.size() + 1) + "] : must be number, is \"" + tokens[2] + "\" ; 50 used by default\n";
        else
            hp = atoi(tokens[2].c_str());

        // HERO SHIELD
        int shield = 0;
        if (tokens[3] != "" && UIManager::IsNumber(tokens[3]) == false)
            parseError += "Invalid Shield value for Hero [" + to_string(_listHeroes.size() + 1) + "] : must be number, is \"" + tokens[3] + "\" ; 0 used by default\n";
        else
            shield = atoi(tokens[3].c_str());

        // HERO WEAPON'S DAMAGE
        int damages = 5;
        if (tokens[4] != "" && UIManager::IsNumber(tokens[4]) == false)
            parseError += "Invalid Shield value for Hero [" + to_string(_listHeroes.size() + 1) + "] : must be number, is \"" + tokens[4] + "\" ; 5 used by default\n";
        else
            damages = atoi(tokens[4].c_str());

        // HERO'S DEFEAT MESSAGE
        string gameOver = tokens[5];

        Hero* pHero = new Hero(heroClass, name, Stats(damages, hp, shield));
        pHero->_gameOver = gameOver;
        _listHeroes.push_back(pHero);
    }

    if (parseError == "") 
        return;
    
    // parsing error
    std::cerr << parseError;
    std::system("pause");
}

string ResourcesManager::GetText(const char* key)
{
    if (_catLanguageResources.find(key) != _catLanguageResources.end())
    {
        if (_catLanguageResources[key] != "")
            return _catLanguageResources[key];
    }
    return key;
}