#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(1251); // Windows-1251 ��� ������
    SetConsoleCP(1251);       // Windows-1251 ��� �����
#endif

    Contact* contacts = NULL;
    unsigned int contact_count = 0;
    const char* filename = "contacts.txt";

    load_from_file(&contacts, &contact_count, filename);

    while (1) {
        printf("\n����:\n");
        printf("1. �������� �������\n");
        printf("2. ������������� �������\n");
        printf("3. ������� �������\n");
        printf("4. ��������� �������� � ����\n");
        printf("5. ��������� �������� �� �����\n");
        printf("6. ������� ��� ��������\n"); // ����� �����
        printf("7. �����\n");               // �������� ����� �� 7
        printf("�������� ��������: ");

        int choice;
        scanf("%d", &choice);
        getchar(); // ������� ������ �������� ������

        switch (choice) {
            case 1:
                add_contact(&contacts, &contact_count);
                break;
            case 2: {
                unsigned int id;
                printf("������� ID �������� ��� ��������������: ");
                scanf("%u", &id);
                getchar();
                edit_contact(contacts, contact_count, id);
                break;
            }
            case 3: {
                unsigned int id;
                printf("������� ID �������� ��� ��������: ");
                scanf("%u", &id);
                getchar();
                delete_contact(&contacts, &contact_count, id);
                break;
            }
            case 4:
                save_to_file(contacts, contact_count, filename);
                break;
            case 5:
                load_from_file(&contacts, &contact_count, filename);
                break;
            case 6: // ����� �����
                print_contacts(contacts, contact_count);
                break;
            case 7: // ����� ������ 7
                free_contacts(contacts, contact_count);
                printf("����� �� ���������.\n");
                return 0;
            default:
                printf("�������� �����, ���������� �����.\n");
        }
    }
    return 0;
}