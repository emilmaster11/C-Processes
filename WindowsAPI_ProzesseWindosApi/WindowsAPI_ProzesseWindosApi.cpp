#include <Windows.h>
#include <iostream>

using namespace std;

int main() {
    DWORD pid = 29620; // Beispiel-PID, ersetzen durch die tatsächliche Prozess-ID
    
    //Erstellt einen Zeiger auf den Prozess
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    //Wenn 0 dann prozess nicht gefunden.
    if (hProcess == NULL) {
        std::cerr << "Failed to open process: " << GetLastError() << '\n';
        return 1;
    }

    // Speicher im Zielprozess reservieren und belegen //LPVOID ist ein Zeiger der auf den reservertieren und belegten speicherplatz zeigt.
    LPVOID addressToWrite = VirtualAllocEx(hProcess, NULL, sizeof(int), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    //Der Neue speicher konnte nicht zugeordnet/belegt werden.
    if (addressToWrite == NULL) {
        std::cerr << "Failed to allocate memory in target process: " << GetLastError() << '\n';
        CloseHandle(hProcess);
        return 1;
    }

    int newValue = 42; // Beispielwert, der in den Prozessspeicher geschrieben werden soll
    
    //Ist ein Datentype der einfach groß genug ist die Byte menge zu speichern.
    SIZE_T bytesWritten;

    //Neuen Speicherplatz um newValue erhöhen.
    BOOL result = WriteProcessMemory(
        hProcess,        // Handle des Zielprozesses
        addressToWrite,  // Adresse im Zielprozess, in die geschrieben werden soll
        &newValue,       // Zeiger auf die Daten, die geschrieben werden sollen
        sizeof(newValue),// Anzahl der Bytes, die geschrieben werden sollen
        
        //WriteProcessMemory verwendet den letzten Parameter, der ein Zeiger auf eine SIZE_T-Variable ist, um die Anzahl der tatsächlich geschriebenen Bytes zurückzugeben
        &bytesWritten    // Zeiger auf die Variable, die die Anzahl der tatsächlich geschriebenen Bytes erhält
        );

    if (!result) {
        std::cerr << "Failed to write to process memory: " << GetLastError() << '\n';
        VirtualFreeEx(hProcess, addressToWrite, 0, MEM_RELEASE); // Speicher freigeben
        CloseHandle(hProcess);
        return 1;
    }
    else {
        cout << bytesWritten<<endl;
        std::cout << "Successfully wrote to process memory.\n";
    }

    // Speicher freigeben
    VirtualFreeEx(hProcess, addressToWrite, 0, MEM_RELEASE);
    CloseHandle(hProcess); // Wichtig, um Ressourcenlecks zu vermeiden
    return 0;
}

