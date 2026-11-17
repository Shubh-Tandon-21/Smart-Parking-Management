
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SMALL 1
#define MEDIUM 2
#define LARGE 3

struct Vehicle {
    char number[20];
    int type;
    int entryHour;
    int entryMinute;
};

struct Slot {
    int id;
    int size;
    int occupied;
    struct Vehicle *vehicle;
};

struct Slot *slots;
int totalSlots = 6;
int nextSlotId = 7;
float totalRevenue = 0;

/* Function to get slot size name */
char *sizeName(int size) {
    if (size == SMALL)
        return "Small";
    if (size == MEDIUM)
        return "Medium";
    return "Large";
}

/* Function to get vehicle type name */
char *vehicleName(int type) {
    if (type == SMALL)
        return "Bike";
    if (type == MEDIUM)
        return "Car";
    return "SUV";
}

/* Display all parking slots */
void displaySlots() {
    int i;

    printf("\n--- PARKING SLOT DETAILS ---\n");
    printf("ID\tSize\t\tStatus\t\tVehicle\n");

    for (i = 0; i < totalSlots; i++) {
        printf("%d\t%-10s\t", slots[i].id,
               sizeName(slots[i].size));

        if (slots[i].occupied)
            printf("Occupied\t%s\n",
                   slots[i].vehicle->number);
        else
            printf("Free\t\t-\n");
    }
}

/* Park vehicle in the smallest suitable free slot */
void parkVehicle() {
    struct Vehicle *v;
    int type, hour, minute, i, j, chosen = -1;
    char number[20];

    printf("\nVehicle type: 1.Bike  2.Car  3.SUV: ");
    scanf("%d", &type);

    if (type < 1 || type > 3) {
        printf("Invalid vehicle type!\n");
        return;
    }

    printf("Enter vehicle number (no spaces): ");
    scanf("%19s", number);

    /* Check if vehicle is already parked */
    for (i = 0; i < totalSlots; i++) {
        if (slots[i].occupied &&
            strcmp(slots[i].vehicle->number, number) == 0) {
            printf("Vehicle is already parked!\n");
            return;
        }
    }

    printf("Enter entry hour (0-23): ");
    scanf("%d", &hour);
    printf("Enter entry minute (0-59): ");
    scanf("%d", &minute);

    if (hour < 0 || hour > 23 ||
        minute < 0 || minute > 59) {
        printf("Invalid entry time!\n");
        return;
    }

    /* Find the smallest suitable free slot */
    for (j = type; j <= LARGE; j++) {
        for (i = 0; i < totalSlots; i++) {
            if (!slots[i].occupied &&
                slots[i].size == j) {
                chosen = i;
                break;
            }
        }
        if (chosen != -1)
            break;
    }

    if (chosen == -1) {
        printf("No suitable free slot available!\n");
        return;
    }

    v = malloc(sizeof(struct Vehicle));

    if (v == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    strcpy(v->number, number);
    v->type = type;
    v->entryHour = hour;
    v->entryMinute = minute;

    slots[chosen].vehicle = v;
    slots[chosen].occupied = 1;

    printf("\nVehicle parked successfully!\n");
    printf("Assigned Slot ID: %d\n", slots[chosen].id);
    printf("Slot Size: %s\n", sizeName(slots[chosen].size));
}

/* Search for a vehicle */
void searchVehicle() {
    char number[20];
    int i;

    printf("Enter vehicle number to search: ");
    scanf("%19s", number);

    for (i = 0; i < totalSlots; i++) {
        if (slots[i].occupied &&
            strcmp(slots[i].vehicle->number, number) == 0) {
            printf("\nVehicle Found!\n");
            printf("Vehicle Number: %s\n",
                   slots[i].vehicle->number);
            printf("Vehicle Type: %s\n",
                   vehicleName(slots[i].vehicle->type));
            printf("Slot ID: %d\n", slots[i].id);
            printf("Entry Time: %02d:%02d\n",
                   slots[i].vehicle->entryHour,
                   slots[i].vehicle->entryMinute);
            return;
        }
    }

    printf("Vehicle not found!\n");
}

/* Calculate parking fee */
float calculateFee(struct Vehicle *v, int exitHour,
                   int exitMinute) {
    int entry, exit, duration, hours, i, currentHour;
    float fee = 0;

    entry = v->entryHour * 60 + v->entryMinute;
    exit = exitHour * 60 + exitMinute;

    duration = exit - entry;

    /* Handle parking overnight */
    if (duration <= 0)
        duration += 24 * 60;

    /* Round up to the next full hour */
    hours = (duration + 59) / 60;

    /* Day rate: Rs. 20/hour; night rate: Rs. 10/hour */
    for (i = 0; i < hours; i++) {
        currentHour = (v->entryHour + i) % 24;

        if (currentHour >= 6 && currentHour < 18)
            fee += 20;
        else
            fee += 10;
    }

    /* Overtime fine after 8 hours */
    if (duration > 8 * 60)
        fee += 100;

    return fee;
}

/* Free the slot and calculate the bill */
void removeVehicle() {
    char number[20];
    int i, hour, minute;
    float fee;

    printf("Enter vehicle number to remove: ");
    scanf("%19s", number);

    for (i = 0; i < totalSlots; i++) {
        if (slots[i].occupied &&
            strcmp(slots[i].vehicle->number, number) == 0) {

            printf("Enter exit hour (0-23): ");
            scanf("%d", &hour);
            printf("Enter exit minute (0-59): ");
            scanf("%d", &minute);

            if (hour < 0 || hour > 23 ||
                minute < 0 || minute > 59) {
                printf("Invalid exit time!\n");
                return;
            }

            fee = calculateFee(slots[i].vehicle,
                               hour, minute);

            printf("\n--- PARKING BILL ---\n");
            printf("Vehicle: %s\n",
                   slots[i].vehicle->number);
            printf("Slot ID: %d\n", slots[i].id);
            printf("Entry: %02d:%02d\n",
                   slots[i].vehicle->entryHour,
                   slots[i].vehicle->entryMinute);
            printf("Exit: %02d:%02d\n", hour, minute);
            printf("Parking Fee: Rs. %.2f\n", fee);

            totalRevenue += fee;

            free(slots[i].vehicle);
            slots[i].vehicle = NULL;
            slots[i].occupied = 0;

            printf("Vehicle removed successfully!\n");
            return;
        }
    }

    printf("Vehicle not found!\n");
}

/* Add two new parking slots using realloc */
void expandParking() {
    struct Slot *temp;
    int i;

    temp = realloc(slots,
                   (totalSlots + 2) * sizeof(struct Slot));

    if (temp == NULL) {
        printf("Unable to expand parking!\n");
        return;
    }

    slots = temp;

    for (i = totalSlots; i < totalSlots + 2; i++) {
        slots[i].id = nextSlotId++;
        slots[i].size = (i % 3) + 1;
        slots[i].occupied = 0;
        slots[i].vehicle = NULL;
    }

    totalSlots += 2;

    printf("Parking expanded successfully!\n");
    printf("Total slots: %d\n", totalSlots);
}

/* Daily occupancy and revenue report */
void dailyReport() {
    int i, occupied = 0, freeSlots;

    for (i = 0; i < totalSlots; i++) {
        if (slots[i].occupied)
            occupied++;
    }

    freeSlots = totalSlots - occupied;

    printf("\n--- PARKING REPORT ---\n");
    printf("Total Slots: %d\n", totalSlots);
    printf("Occupied Slots: %d\n", occupied);
    printf("Free Slots: %d\n", freeSlots);
    printf("Occupancy: %.2f%%\n",
           (occupied * 100.0) / totalSlots);
    printf("Revenue Collected: Rs. %.2f\n", totalRevenue);
}

/* Main function */
int main() {
    int choice, i;

    slots = malloc(totalSlots * sizeof(struct Slot));

    if (slots == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    /* Initialize six parking slots */
    for (i = 0; i < totalSlots; i++) {
        slots[i].id = i + 1;
        slots[i].size = (i % 3) + 1;
        slots[i].occupied = 0;
        slots[i].vehicle = NULL;
    }

    do {
        printf("\n===== SMART PARKING MANAGEMENT =====\n");
        printf("1. Display Parking Slots\n");
        printf("2. Park Vehicle\n");
        printf("3. Search Vehicle\n");
        printf("4. Remove Vehicle and Calculate Bill\n");
        printf("5. Expand Parking Slots\n");
        printf("6. Daily Report\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displaySlots();
                break;

            case 2:
                parkVehicle();
                break;

            case 3:
                searchVehicle();
                break;

            case 4:
                removeVehicle();
                break;

            case 5:
                expandParking();
                break;

            case 6:
                dailyReport();
                break;

            case 7:
                printf("Thank you for using Smart Parking!\n");
                break;

            default:
                printf("Invalid choice! Try again.\n");
        }
    } while (choice != 7);

    /* Release all allocated memory */
    for (i = 0; i < totalSlots; i++) {
        if (slots[i].occupied)
            free(slots[i].vehicle);
    }

    free(slots);

    return 0;
}
