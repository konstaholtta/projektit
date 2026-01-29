// Choose the file comment based on your preferred language.

/* Kasvien hoito-ohjeet
*
* Ohjelman kuvaus:
* Ohjelma lukee tietoja syötetiedostosta, jossa luetellaan huoneittain
* kasvien kastelutiheys päivissä. Lisäksi kasveista mainitaan ruukun väri
* niiden tunnistamisen helpottamiseksi. Kasvien tiedoista on mahdollista
* koostaa kasteluaikataulu, jossa jokaiselle päivälle merkitään, mitä kasveja
* kunakin päivänä tulisi kastella.
*
* Ohjelman käyttö:
* Tiedostosta löytyvät tiedot kerätään [sopivaan tietorakenteeseen], joka koostuu
* STL:n säiliöistä ja tietueista. Tämän jälkeen käyttäjä voi tehdä hakuja
* tietorekenteeseen käyttämällä komentoja "ls", "room", "add", "mv" ja
* "sch". Komennoilla on eri määrä parametreja, ja ohjelma tarkistaa, että
* niitä on sekä oikea määrä että niiden oikeellisuuden.
*
* Ohjelman kirjoittajatiedot: (Täydennä omasi!)
* Nimi: Teemu Teekkari
* Opiskelijanumero: 1234567
* Käyttäjänimi: teekkart
* Sähköposti: teemu.teekkari@tuni.fi
*
* Perusteluita toteutusvalinnoille: (Täydennä asiaankuuluvasti!)
*   (Valitsin tietorakenteeksi rakenteen X, jossa yhdistellään ...
*   Nämä kuvaavat ohjelmassa toimintoja X ....)
*
* Muita huomioita: (Täydennä tarvittaessa)
* - Käyttämäsi tekoälytyökalut (työkalu, käyttötarkoitus). Ole yksityiskohtainen!
* - Mahdollisia kysymyksiä jne.
*/

/* Plant watering instructions
 *
 * Program description:
 * The program reads data from an input file that lists, by room,
 * the watering intervals of plants in days. In addition, the color
 * of each plant’s pot is given to make identifying the plants easier.
 * Based on the plant data, a watering schedule can be generated,
 * indicating which plants should be watered on each day.
 *
 * Program usage:
 * The data read from the file is stored in a [suitable data structure]
 * consisting of STL containers and structs. After this, the user can
 * search the data structure using the commands "ls", "room", "add",
 * "mv", and "sch". The commands take a varying number of parameters,
 * and the program checks both that the correct number of parameters
 * is given and that the parameters themselves are valid.
 *
 * Author information: (Fill in your own details!)
 * Name: Teemu Teekkari
 * Student number: 1234567
 * Username: teekkart
 * Email: teemu.teekkari@tuni.fi
 *
 * Justification of implementation choices: (Fill in as appropriate!)
 *   (For example: I chose a data structure X that combines ...
 *   These structures represent the program’s functionality X ...)
 *
 * Additional notes: (Fill in if necessary)
 * - AI tools used (tool, purpose). Be specific!
 * - Possible questions, remarks, etc.
 */


#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>

using namespace std;

// Enum class to be used for file handling.
enum class DATA_FIELDS { ROOM, PLANT, WATERING_PERIOD, FERTILIZATION_PERIOD,
                         FIELDS };

// Error messages for file handling.
const string ERROR_FILE = "Error: Unable to open file.";
const string ERROR_FIELDS = "Error: Invalid number of fields in data file.";
const string ERROR_PERIOD = "Error: Invalid expression of a number in the data file.";

// Enum class and vector to be used for weekday handling.
// You can index the vector by using the corresponding enum value.
enum class DAY { MONDAY, TUESDAY, WEDNESDAY, THURSDAY,
                 FRIDAY, SATURDAY, SUNDAY, DAYS_A_WEEK };

const vector<string> WEEKDAYS = { "Monday", "Tuesday", "Wednesday",
                                 "Thursday", "Friday", "Saturday", "Sunday" };

// Enum class and command word map to be used for command handling.
// You can access the correct enum command value depending on the command words
// listed on the COMMANDS map, and check the needed amount of parameters from the
// COMMAND_PARAMS map.
enum class COMMAND { HELP, LIST_ALL, LIST_ROOM, PRINT_SCHEDULE, MOVE_PLANT,
                     ADD_PLANT, QUIT };

const map<string, COMMAND> COMMANDS = { { "help", COMMAND::HELP },
                                        { "ls", COMMAND:: LIST_ALL },
                                        { "list all", COMMAND::LIST_ALL },
                                        { "room", COMMAND::LIST_ROOM },
                                        { "list room", COMMAND::LIST_ROOM },
                                        { "sch", COMMAND::PRINT_SCHEDULE },
                                        { "mv", COMMAND::MOVE_PLANT },
                                        { "add", COMMAND::ADD_PLANT },
                                        { "quit", COMMAND::QUIT },
                                        { "q", COMMAND::QUIT } };

const map<COMMAND, size_t> COMMAND_PARAMS = { { COMMAND::HELP, 0 },
                                              { COMMAND::LIST_ALL, 0 },
                                              { COMMAND::LIST_ROOM, 1 },
                                              { COMMAND::PRINT_SCHEDULE, 0 },
                                              { COMMAND::MOVE_PLANT, 4 },
                                              { COMMAND::ADD_PLANT, 4 },
                                              { COMMAND::QUIT, 0 } };

// Error messages for command handling and commands.
const string ERROR_COMMAND = "Error: Command not found.";
const string ERROR_PARAMETERS = "Error: Wrong number of parameters.";
const string ERROR_DAY = "Error: Incorrect expression of a day.";
const string ERROR_ROOM = "Error: Incorrect room information.";
const string ERROR_ADD_PLANT = "Error: Unexpected input for plant information.";
const string ERROR_MOVE_PLANT = "Error: Incorrect room or plant information.";

const string SUCCESS_ADD_PLANT = "Plant added succesfully.";
const string SUCCESS_MOVE_PLANT = "Plant moved succesfully.";

// All command prinouts follow the same indentation structure given here.
const string INDENT_1 = ":: ";
const string INDENT_2 = "  <> ";
const string INDENT_3 = "    + ";
const string INDENT_4 = "    ... ";

// The Plant-struct is used to save infromation related to plants. Each
// row of the input file will be saved as a separate Plant.
struct Plant
{
    string name = "";
    int watering_period = 1;
    string pot_color = "";

    // The plants are defined to be the same, if all of the decriptive values
    // match: the name, the watering period and the pot color.
    bool operator== (const Plant& other) const
    {
        return name == other.name and
               watering_period == other.watering_period and
               pot_color == other.pot_color;
    };

    // The plants are ordered depending first on the name, then the
    // watering period and at last (if needed), the pot color.
    bool operator<(const Plant& other) const
    {
        if ( name != other.name )
        {
            return name < other.name;
        }
        if ( watering_period != other.watering_period )
        {
            return watering_period < other.watering_period;
        }
        return pot_color < other.pot_color;
    }
};

// Choose a suitable container and define it here.
// Note: Don't *initialize* the structure here, but within the main function.
using Plant_info = /*.. Write your structure definition here ..*/;


int main()
{
    cout << "~~ Plant care instructions ~~" << endl;

    // Data structure declaration
    Plant_info info;

    // TODO: Write code or more preferable call functions for
    // - asking for an input file
    // - reading the content of the file into the data structure
    // - processing user-given commands

    return EXIT_SUCCESS;
}
