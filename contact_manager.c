// Hello Bro this is a my c language programm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 50
#define MAX_PHONE 20
#define MAX_EMAIL 50
#define MAX_ADDRESS 100
#define MAX_CONTACTS 1000
#define FILENAME "contacts.dat"

typedef struct
{
    int id;
    char name[MAX_NAME];
    char phone[MAX_PHONE];
    char email[MAX_EMAIL];
    char address[MAX_ADDRESS];
} Contact;

typedef struct
{
    Contact contacts[MAX_CONTACTS];
    int count;
    int next_id;
} ContactManager;

// Function prototypes
void initialize_manager(ContactManager *manager);
void load_contacts(ContactManager *manager);
void save_contacts(ContactManager *manager);
void display_menu();
void add_contact(ContactManager *manager);
void view_all_contacts(ContactManager *manager);
void search_contact(ContactManager *manager);
void edit_contact(ContactManager *manager);
void delete_contact(ContactManager *manager);
void display_contact(Contact *contact);
void clear_input_buffer();
void to_lower(char *str);
int find_contact_by_id(ContactManager *manager, int id);
int find_contact_by_name(ContactManager *manager, char *name);

int main()
{
    ContactManager manager;
    int choice;

    initialize_manager(&manager);
    load_contacts(&manager);

    printf("=== Contact Management System ===\n");
    printf("Welcome! Your contacts have been loaded.\n\n");

    while (1)
    {
        display_menu();
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice)
        {
        case 1:
            add_contact(&manager);
            break;
        case 2:
            view_all_contacts(&manager);
            break;
        case 3:
            search_contact(&manager);
            break;
        case 4:
            edit_contact(&manager);
            break;
        case 5:
            delete_contact(&manager);
            break;
        case 6:
            save_contacts(&manager);
            printf("Thank you for using Contact Management System!\n");
            exit(0);
        default:
            printf("Invalid choice! Please try again.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
    }

    return 0;
}

void initialize_manager(ContactManager *manager)
{
    manager->count = 0;
    manager->next_id = 1;
}

void load_contacts(ContactManager *manager)
{
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL)
    {
        printf("No existing contact file found. Starting fresh.\n");
        return;
    }

    fread(&manager->count, sizeof(int), 1, file);
    fread(&manager->next_id, sizeof(int), 1, file);
    fread(manager->contacts, sizeof(Contact), manager->count, file);

    fclose(file);
    printf("Loaded %d contacts from file.\n", manager->count);
}

void save_contacts(ContactManager *manager)
{
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL)
    {
        printf("Error: Unable to save contacts to file.\n");
        return;
    }

    fwrite(&manager->count, sizeof(int), 1, file);
    fwrite(&manager->next_id, sizeof(int), 1, file);
    fwrite(manager->contacts, sizeof(Contact), manager->count, file);

    fclose(file);
    printf("Contacts saved successfully.\n");
}

void display_menu()
{
    printf("\n=== CONTACT MANAGEMENT MENU ===\n");
    printf("1. Add New Contact\n");
    printf("2. View All Contacts\n");
    printf("3. Search Contact\n");
    printf("4. Edit Contact\n");
    printf("5. Delete Contact\n");
    printf("6. Save and Exit\n");
    printf("================================\n");
}

void add_contact(ContactManager *manager)
{
    if (manager->count >= MAX_CONTACTS)
    {
        printf("Error: Maximum contact limit reached!\n");
        return;
    }

    Contact *new_contact = &manager->contacts[manager->count];
    new_contact->id = manager->next_id++;

    printf("\n=== ADD NEW CONTACT ===\n");

    printf("Enter Name: ");
    fgets(new_contact->name, MAX_NAME, stdin);
    new_contact->name[strcspn(new_contact->name, "\n")] = 0; // Remove newline

    printf("Enter Phone: ");
    fgets(new_contact->phone, MAX_PHONE, stdin);
    new_contact->phone[strcspn(new_contact->phone, "\n")] = 0;

    printf("Enter Email: ");
    fgets(new_contact->email, MAX_EMAIL, stdin);
    new_contact->email[strcspn(new_contact->email, "\n")] = 0;

    printf("Enter Address: ");
    fgets(new_contact->address, MAX_ADDRESS, stdin);
    new_contact->address[strcspn(new_contact->address, "\n")] = 0;

    manager->count++;
    printf("Contact added successfully! (ID: %d)\n", new_contact->id);
}

void view_all_contacts(ContactManager *manager)
{
    if (manager->count == 0)
    {
        printf("\nNo contacts found.\n");
        return;
    }

    printf("\n=== ALL CONTACTS (%d) ===\n", manager->count);
    for (int i = 0; i < manager->count; i++)
    {
        printf("\n--- Contact %d ---\n", i + 1);
        display_contact(&manager->contacts[i]);
    }
}

void search_contact(ContactManager *manager)
{
    if (manager->count == 0)
    {
        printf("\nNo contacts to search.\n");
        return;
    }

    char search_term[MAX_NAME];
    printf("\n=== SEARCH CONTACT ===\n");
    printf("Enter name to search: ");
    fgets(search_term, MAX_NAME, stdin);
    search_term[strcspn(search_term, "\n")] = 0;

    to_lower(search_term);

    printf("\nSearch Results:\n");
    int found = 0;

    for (int i = 0; i < manager->count; i++)
    {
        char temp_name[MAX_NAME];
        strcpy(temp_name, manager->contacts[i].name);
        to_lower(temp_name);

        if (strstr(temp_name, search_term) != NULL)
        {
            printf("\n--- Match Found ---\n");
            display_contact(&manager->contacts[i]);
            found++;
        }
    }

    if (found == 0)
    {
        printf("No contacts found matching '%s'.\n", search_term);
    }
    else
    {
        printf("\nFound %d matching contact(s).\n", found);
    }
}

void edit_contact(ContactManager *manager)
{
    if (manager->count == 0)
    {
        printf("\nNo contacts to edit.\n");
        return;
    }

    int id;
    printf("\n=== EDIT CONTACT ===\n");
    printf("Enter Contact ID to edit: ");

    if (scanf("%d", &id) != 1)
    {
        printf("Invalid input!\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int index = find_contact_by_id(manager, id);
    if (index == -1)
    {
        printf("Contact with ID %d not found.\n", id);
        return;
    }

    Contact *contact = &manager->contacts[index];

    printf("\nCurrent Contact Information:\n");
    display_contact(contact);

    printf("\nEnter new information (press Enter to keep current value):\n");

    char temp[100];

    printf("Name [%s]: ", contact->name);
    fgets(temp, sizeof(temp), stdin);
    if (strlen(temp) > 1)
    {
        temp[strcspn(temp, "\n")] = 0;
        strcpy(contact->name, temp);
    }

    printf("Phone [%s]: ", contact->phone);
    fgets(temp, sizeof(temp), stdin);
    if (strlen(temp) > 1)
    {
        temp[strcspn(temp, "\n")] = 0;
        strcpy(contact->phone, temp);
    }

    printf("Email [%s]: ", contact->email);
    fgets(temp, sizeof(temp), stdin);
    if (strlen(temp) > 1)
    {
        temp[strcspn(temp, "\n")] = 0;
        strcpy(contact->email, temp);
    }

    printf("Address [%s]: ", contact->address);
    fgets(temp, sizeof(temp), stdin);
    if (strlen(temp) > 1)
    {
        temp[strcspn(temp, "\n")] = 0;
        strcpy(contact->address, temp);
    }

    printf("Contact updated successfully!\n");
}

void delete_contact(ContactManager *manager)
{
    if (manager->count == 0)
    {
        printf("\nNo contacts to delete.\n");
        return;
    }

    int id;
    char confirm;

    printf("\n=== DELETE CONTACT ===\n");
    printf("Enter Contact ID to delete: ");

    if (scanf("%d", &id) != 1)
    {
        printf("Invalid input!\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int index = find_contact_by_id(manager, id);
    if (index == -1)
    {
        printf("Contact with ID %d not found.\n", id);
        return;
    }

    printf("\nContact to be deleted:\n");
    display_contact(&manager->contacts[index]);

    printf("\nAre you sure you want to delete this contact? (y/N): ");
    confirm = getchar();
    clear_input_buffer();

    if (confirm == 'y' || confirm == 'Y')
    {
        // Shift all contacts after the deleted one
        for (int i = index; i < manager->count - 1; i++)
        {
            manager->contacts[i] = manager->contacts[i + 1];
        }
        manager->count--;
        printf("Contact deleted successfully!\n");
    }
    else
    {
        printf("Delete operation cancelled.\n");
    }
}

void display_contact(Contact *contact)
{
    printf("ID: %d\n", contact->id);
    printf("Name: %s\n", contact->name);
    printf("Phone: %s\n", contact->phone);
    printf("Email: %s\n", contact->email);
    printf("Address: %s\n", contact->address);
}

void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void to_lower(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

int find_contact_by_id(ContactManager *manager, int id)
{
    for (int i = 0; i < manager->count; i++)
    {
        if (manager->contacts[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

int find_contact_by_name(ContactManager *manager, char *name)
{
    for (int i = 0; i < manager->count; i++)
    {
        if (strcmp(manager->contacts[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}