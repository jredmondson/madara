package ai.madara.exceptions;

public class MadaraDeadObjectException extends Exception {

	/**
	 * version
	 */
	private static final long serialVersionUID = 1L;

    /**
     * Constructor
     */
	public MadaraDeadObjectException() {
	}

    /**
     * Constructor with message
     * @param  message information to embed in the exception
     */
	public MadaraDeadObjectException(String message) {
		super(message);
	}

    /**
     * Constructor with cause
     * @param  cause  source of the exception
     */
	public MadaraDeadObjectException(Throwable cause) {
		super(cause);
	}

    /**
     * Constructor with cause and message
     * @param  message information to embed in the exception
     * @param  cause  source of the exception
     */
	public MadaraDeadObjectException(String message, Throwable cause) {
		super(message, cause);
	}

    /**
     * Constructor with cause and message and suppression
     * @param  message information to embed in the exception
     * @param  cause  source of the exception
     * @param  enableSuppression  if true, suppress the throw
     * @param  writableStackTrace  if true, write stack trace
     */
	public MadaraDeadObjectException(String message, Throwable cause, boolean enableSuppression,
			boolean writableStackTrace) {
		super(message, cause, enableSuppression, writableStackTrace);
	}

}
