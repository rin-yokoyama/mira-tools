from kafka import KafkaConsumer
import pyarrow as pa
import pyarrow.ipc as ipc
import io

# Kafka consumer configuration
topic_name = 'decoded'
bootstrap_servers = '192.168.0.2:9092'

# Create a Kafka consumer
consumer = KafkaConsumer(
    topic_name,
    bootstrap_servers=bootstrap_servers,
    auto_offset_reset='latest'
)

for message in consumer:
    # The message value is expected to be a byte buffer containing serialized Arrow data
    buffer = io.BytesIO(message.value)
    print(buffer)

    # Use Arrow IPC to read the serialized RecordBatch
    reader = ipc.open_stream(buffer)
    batch = reader.read_next_batch()
    
    # Optionally convert the RecordBatch to a Table if you need to work with Table API
    table = pa.Table.from_batches([batch])
    
    # Example of processing the data
    #print(table)
    print(table.to_pandas())  # Convert to pandas DataFrame for easier manipulation

# Close the consumer when done
consumer.close()