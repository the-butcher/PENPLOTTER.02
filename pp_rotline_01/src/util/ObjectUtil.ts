

export class ObjectUtil {

    /**
     * create a unique 12-digit id
     * @returns
     */
    static createId(): string {
        return Math.round(Math.random() * 10000000000).toString(16).substring(0, 12);
    }

}